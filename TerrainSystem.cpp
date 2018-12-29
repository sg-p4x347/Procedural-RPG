#include "pch.h"
#include "TerrainSystem.h"
#include "Terrain.h"
#include "Position.h"
#include "PositionNormalTextureTangentColorVBO.h"
#include "Movement.h"
#include "ProUtil.h"
#include "Utility.h"
#include "Tag.h"
#include "TaskThread.h"
#include "WaterCell.h"
#include "Model.h"
#include "VoxelGridModel.h"
#include <thread>
#include "TopologyCruncher.h"
#include "VboParser.h"
#include "AssetManager.h"
#include "TreeGenerator.h"
#include "SystemManager.h"
//#include "ActionSystem.h"
#include "BuildingSystem.h"
#include "TaskManager.h"
#include "Inventory.h"
#include "IEventManager.h"
#include "Movement.h"
#include "Global.h"
//#include "ItemSystem.h"

using namespace DirectX::SimpleMath;
static EntityPtr waterEntity;
namespace world {
	TerrainSystem::TerrainSystem(
		SystemManager * systemManager,
		WEM *  entityManager,
		unsigned short updatePeriod,
		int worldWidth,
		int regionWidth,
		Filesystem::path directory
	)
		: WorldSystem::WorldSystem(entityManager, updatePeriod),
		SM(systemManager),
		m_width(worldWidth),
		m_regionWidth(regionWidth)
	{
		m_directory = directory / Name();
		Filesystem::create_directory(m_directory);
		RegisterHandlers();
		//m_workerThread = TaskThread()
		//m_workers = Map<std::thread>(m_width / m_regionWidth,0,0,0);


	}


	TerrainSystem::~TerrainSystem()
	{
	}

	void TerrainSystem::Generate(std::function<void(float,std::string)> && progressCallback)
	{
		JsonParser config(std::ifstream("config/continent.json"));
		// Mountain settings
		JsonParser mountains = config["mountains"];
		m_biomeAmplitude = mountains["biomeAmplitude"].To<double>();
		m_biomeShift = mountains["biomeShift"].To<double>();
		m_biomeWidth = mountains["biomeWidth"].To<double>();
		m_continentShift = mountains["continentShift"].To<double>();
		m_continentWidth = mountains["continentWidth"].To<double>();
		// General
		m_sampleSpacing = config["sampleSpacing"].To<int>();
		const float landAmplitude = config["landAmplitude"].To<double>();
		const float oceanAmplitude = config["oceanAmplitude"].To<double>();
		const float biomeDeviationConst = config["biomeDeviationConst"].To<double>();

		// Terrain map
		JsonParser terrainMap = config["terrainMap"];
		TerrainMap.reset(new HeightMap(m_width, m_width * terrainMap["initialDeviation"].To<double>(), terrainMap["deviationDecrease"].To<double>(), 0));
		// Continent map
		JsonParser continentMap = config["continentMap"];
		const float CM_offset = continentMap["offset"].To<double>();
		HeightMap continent = HeightMap(TerrainMap->width / m_sampleSpacing, (TerrainMap->width / m_sampleSpacing) * continentMap["initialDeviation"].To<double>(), continentMap["deviationDecrease"].To<double>(), continentMap["zoom"].To<int>());
		// Biome map
		JsonParser biomeMap = config["biomeMap"];
		HeightMap biome = HeightMap(TerrainMap->width / m_sampleSpacing, biomeMap["initialDeviation"].To<double>(), biomeMap["deviationDecrease"].To<double>(), biomeMap["zoom"].To<int>());

		//Load(); TEMP
		// initialize the corners
		for (int x = 0; x <= continent.width; x += continent.width) {
			for (int y = 0; y <= continent.width; y += continent.width) {
				continent.map[x][y] = -continent.initialDeviation;
				biome.map[x][y] = Deviation(biome.initialDeviation);
				float continentZ = (continent.map[x][y] + (CM_offset * continent.initialDeviation)) * m_sampleSpacing;
				TerrainMap->map[x*m_sampleSpacing][y*m_sampleSpacing] = continentZ * oceanAmplitude + BiomeDeviation(biome.map[x][y], continentZ);
			}
		}
		//auto start = std::chrono::high_resolution_clock::now();
		//auto end = std::chrono::high_resolution_clock::now();
		//auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		// iterate
		for (int iteration = 0; iteration < floor(log2(TerrainMap->width)); iteration++) {
			int gridWidth = (TerrainMap->width / pow(2, iteration)) / 2;
			// decrease deviation for the decrease in distance between points
			if (iteration >= continent.zoom) {
				continent.squareDeviation = continent.diamondDeviation * 1.4f;//0.707106781f;
			}
			if (iteration >= biome.zoom) {
				biome.squareDeviation = biome.diamondDeviation * 1.4f;//0.707106781f;
			}
			TerrainMap->squareDeviation = TerrainMap->diamondDeviation * 1.4f;//0.707106781f;
			for (int x = gridWidth; x < TerrainMap->width; x += gridWidth * 2) {
				for (int y = gridWidth; y < TerrainMap->width; y += gridWidth * 2) {
					//-----------
					// Diamond
					//-----------
					int sampleX = x / m_sampleSpacing;
					int sampleY = y / m_sampleSpacing;
					int sampleGridWidth = gridWidth / m_sampleSpacing;
					// Continent map
					continent.map[sampleX][sampleY] = iteration == 0 ? continent.initialDeviation : Diamond(continent, sampleX, sampleY, sampleGridWidth) + Deviation(continent.diamondDeviation);
					// Biome map
					biome.map[sampleX][sampleY] = Diamond(biome, sampleX, sampleY, sampleGridWidth) + Deviation(biome.diamondDeviation);
					// Terrain map
					float continentSample = (continent.map[sampleX][sampleY] + (CM_offset * continent.initialDeviation)) * m_sampleSpacing;
					float biomeSample = biome.map[sampleX][sampleY];
					continentSample *= continentSample > 0 ? landAmplitude : oceanAmplitude; // scale the point for land or ocean
					float biomeDeviation = BiomeDeviation(biomeSample, continentSample);
					if (gridWidth >= m_sampleSpacing) {
						TerrainMap->map[x][y] = continentSample;
						TerrainMap->map[x][y] += biomeDeviation;
						if (iteration > 2) {
							TerrainMap->map[x][y] += (TerrainMap->diamondDeviation * Deviation(biomeDeviation)*biomeDeviationConst);
						}
					}
					else {
						TerrainMap->map[x][y] = Diamond(*TerrainMap, x, y, gridWidth);
						TerrainMap->map[x][y] += Deviation(TerrainMap->diamondDeviation);
						TerrainMap->map[x][y] += (TerrainMap->diamondDeviation * Deviation(biomeDeviation)*biomeDeviationConst);
					}
					//-----------
					// Square
					//-----------
					for (float rad = (x == TerrainMap->width - gridWidth || y == TerrainMap->width - gridWidth ? 0.f : 2.f); rad < 4.f; rad++) {
						int pointX = round(x + cos(rad*XM_PI / 2) * gridWidth);
						int pointY = round(y + sin(rad*XM_PI / 2) * gridWidth);
						sampleX = pointX / m_sampleSpacing;
						sampleY = pointY / m_sampleSpacing;
						// Continent map
						if (sampleX == 0 || sampleX == continent.width || sampleY == 0 || sampleY == continent.width) {
							// point is on the edge of the mountain
							continent.map[sampleX][sampleY] = -continent.initialDeviation;
						}
						else {
							// use the square method to calculate the elevation
							continent.map[sampleX][sampleY] = Square(continent, sampleX, sampleY, sampleGridWidth);
							continent.map[sampleX][sampleY] += Deviation(continent.squareDeviation);
						}
						// Biome map
						biome.map[sampleX][sampleY] = Square(biome, sampleX, sampleY, sampleGridWidth);
						biome.map[sampleX][sampleY] += Deviation(biome.squareDeviation);
						// Terrain map
						float continentSquareSample = (continent.map[sampleX][sampleY] + (CM_offset * continent.initialDeviation)) * m_sampleSpacing;
						float biomeSquareSample = biome.map[sampleX][sampleY];
						continentSquareSample *= continentSquareSample > 0 ? landAmplitude : oceanAmplitude; // scale the point for land or ocean
						float biomeDeviation = BiomeDeviation(biomeSquareSample, continentSquareSample);
						if (gridWidth >= m_sampleSpacing) {
							TerrainMap->map[pointX][pointY] = continentSquareSample;
							TerrainMap->map[pointX][pointY] += biomeDeviation;
							if (iteration > 2) {
								TerrainMap->map[pointX][pointY] += (TerrainMap->squareDeviation *Deviation(biomeDeviation)*biomeDeviationConst);
							}
						}
						else {
							TerrainMap->map[pointX][pointY] = Square(*TerrainMap, pointX, pointY, gridWidth);
							TerrainMap->map[pointX][pointY] += Deviation(TerrainMap->squareDeviation);
							TerrainMap->map[pointX][pointY] += (TerrainMap->squareDeviation *Deviation(biomeDeviation)*biomeDeviationConst);
						}
					}
				}
			}
			// decrease the random deviation range
			if (iteration >= continent.zoom) {
				continent.diamondDeviation *= (0.5f * continent.deviationDecrease);
			}
			if (iteration >= biome.zoom) {
				biome.diamondDeviation *= (0.5f * biome.deviationDecrease);
			}
			TerrainMap->diamondDeviation *= (0.5f * TerrainMap->deviationDecrease);
		}

		// Erosion
		bool erosion = config["erosion"].To<bool>();
		if (erosion) UpdateDroplets(*TerrainMap, InitializeDroplets(), InitializeThermalErosionMap(), [=](float percent) {
			progressCallback(percent * 0.5, "Erosion Simulation ...");
		});
		// Terrain done yay!
		Utility::OutputLine("Generating Buildings...");
		Rectangle footprint = Rectangle(20, 20, ProUtil::RandWithin(6, 10), ProUtil::RandWithin(6, 10));
		Rectangle flattenArea = Rectangle(footprint.x - 1, footprint.y - 1, footprint.width + 2, footprint.height + 2);
		Rectangle cacheArea = Rectangle(flattenArea.x - 10, flattenArea.y - 10, flattenArea.width + 20, flattenArea.height + 20);
		//HeightMap cache = HeightMap(cacheArea);
		//ImportMap(cache);
		float height = Flatten(*TerrainMap, flattenArea, 10);
		SM->GetSystem<BuildingSystem>("Building")->CreateBuilding(Vector3(footprint.x, height + 5.f, footprint.y), Rectangle(0, 0, footprint.width, footprint.height), "residential");

		AssetManager::Get()->CreateHeightMapModel("terrain", TerrainMap.get(), AssetManager::Get()->CreateNormalMap(TerrainMap.get()), 10.f, m_regionWidth, "Terrain");
		CreateTerrainEntities();
		//SaveTerrain(*TerrainMap,biome);


		// Rain simulation
		InitializeErosionMap();
		if (erosion) UpdateWater(*TerrainMap, *WaterMap, [=](float percent) {
			progressCallback(0.5 + percent * 0.5, "Water Simulation ...");
		});
		// Water done yay!
		
		shared_ptr<HeightMap> waterMap = std::make_shared<HeightMap>(WaterMap->width, WaterMap->length);
		// copy water cell height values to the heightmap
		for (int x = 0; x < WaterMap->width;x++) {
			for (int z = 0; z < WaterMap->length;z++) {
				waterMap->map[x][z] = WaterMap->map[x][z].Water;
			}
		}
		AssetManager::Get()->CreateHeightMapModel("water", waterMap.get(), AssetManager::Get()->CreateNormalMap(waterMap->width, waterMap->length, [&](int x, int y) {
			return waterMap->map[x][y] + TerrainMap->map[x][y];
		}), 10.f, m_regionWidth, "Water");
		//CreateWaterEntities();
		//SaveWater(*WaterMap);

		// Resources
		//CreateResourceEntities();

		// TEMP
		EntityPtr asset;
		if (AssetManager::Get()->GetStaticEM()->TryFindByPathID("Crate",asset)) {
			int offset = 0;
			int max = 1;
			int spacing = 1;
			for (int x = offset; x < offset + max; x += spacing) {
				for (int z = offset; z < offset + max; z += spacing) {
					EM->CreateEntity(
						Position(Vector3(x, TerrainMap->Height(x, z), z)),
						Model(asset->ID(), AssetType::Authored),
						Collision(asset->ID(), AssetType::Authored)
					);
				}
			}
			
		}
		//SM->GetSystem<ItemSystem>("Item")->NewContainer(Vector3(32, TerrainMap->Height(32, 32), 32), Vector3::Zero, "Crate");

		
		//Save(cache);
		AssetManager::Get()->GetProceduralEM()->Save();
	}

	void TerrainSystem::Update(double & elapsed)
	{
		/*Vector3 velocity = EM->Player()->GetComponent<Components::Movement>("Movement")->Velocity;
		Vector3 position = EM->PlayerPos()->Pos;
		if (velocity.Length() < 100) {
			UpdateRegions(position);
		}*/
	}

	string TerrainSystem::Name()
	{
		return "Terrain";
	}

	void TerrainSystem::SyncEntities()
	{
	}

	string TerrainSystem::GetBiomeName(float sample)
	{
		JsonParser continentCfg("config/continent.json");
		vector<string> biomeNames = continentCfg["biomes"].GetKeys();
		for (string biomeName : biomeNames) {
			if (abs(continentCfg["biomes"][biomeName]["center"].To<double>() - sample) <= continentCfg["biomes"][biomeName]["range"].To<double>()) {
				return biomeName;
			}
		}
		return "";
	}

	float TerrainSystem::Height(const int & x, const int & z)
	{
		ifstream terrainStream(m_directory / "terrain.dat", ios::binary);
		if (terrainStream.is_open()) {
			int index = Utility::posToIndex(x, z, m_width + 1);
			return InternalHeight(terrainStream, index, 10.f);
		}
		return 0.0;
	}

	float TerrainSystem::Height(float & x, float & z)
	{
		return m_cache ? m_cache->Height(x - m_cache->area.x, z - m_cache->area.y) : 0.f;
	}

	int TerrainSystem::Width()
	{
		return m_width;
	}

	DirectX::SimpleMath::Rectangle TerrainSystem::Area()
	{
		return DirectX::SimpleMath::Rectangle(0, 0, Width(), Width());
	}

	void TerrainSystem::SetVertex(const int & x, const int & z, const float value)
	{
		/*vector<EntityPtr> regions = EM->FindEntities(EM->ComponentMask("Terrain"));
		for (auto & region : regions) {
			shared_ptr<Components::Position> pos = EM->GetComponent<Components::Position>(region, "Position");

		}
		shared_ptr<Components::VBO> terrainVBO = EM->GetComponent<Components::VBO>(region, "VBO");

		if (terrainVBO->Vertices.size() != 0) terrainVBO->Vertices[Utility::posToIndex(z, x, m_width + 1)].position.y = water.map[x][z].Height;*/
	}

	void TerrainSystem::SaveTerrain(HeightMap & terrain, HeightMap & biome)
	{
		//-------------------------------
		// save terrain to binary file
		//-------------------------------

		// Terrain Map
		unsigned int vertexCount = (terrain.width + 1) * (terrain.width + 1);
		unique_ptr<unsigned char[]> terrainBuffer(new unsigned char[vertexCount * sizeof(short)]);

		// Normal Map
		unique_ptr<char[]> normalBuffer(new char[vertexCount * 3]); // X (8 bits) Y (8 bits) Z (8 bits)

																	// Biome Map
		unsigned int sampleSpacing = terrain.width / biome.width;
		unsigned int sampleCount = (biome.width + 1) * (biome.width + 1);
		unique_ptr<unsigned char[]> biomeBuffer(new unsigned char[sampleCount * sizeof(float)]);



		unsigned int index = 0;
		unsigned int biomeIndex = 0;

		for (unsigned short vertZ = 0; vertZ <= terrain.width; vertZ++) {
			for (unsigned short vertX = 0; vertX <= terrain.width; vertX++) {
				// verticies
				float vertex = terrain.map[vertX][vertZ];
				short vertexShort = (short)(vertex * 10.f);
				/*char lower = vertexShort & 0xff;
				char upper = (vertexShort >> 8) & 0xff;
				terrainBuffer.get()[index * sizeof(short)] = lower;
				terrainBuffer.get()[index * sizeof(short) + 1] = upper;*/
				std::memcpy(&terrainBuffer.get()[index * sizeof(short)], &vertexShort, sizeof(short));
				// biome
				if (vertZ % sampleSpacing == 0 && vertX % sampleSpacing == 0) {
					float floatValue = biome.map[vertX / sampleSpacing][vertZ / sampleSpacing];
					std::memcpy(&biomeBuffer.get()[biomeIndex * sizeof(float)], &floatValue, sizeof(float));
					/*biomeFile.seekp(biomeIndex * sizeof(float));
					biomeFile.write((char *)charValue, sizeof(float));*/
					biomeIndex++;
				}
				// normals
				float left = signed(vertX) - 1 >= 0 ? terrain.map[vertX - 1][vertZ] : vertex;
				float right = signed(vertX) + 1 <= signed(terrain.width) ? terrain.map[vertX + 1][vertZ] : vertex;
				float up = signed(vertZ) + 1 <= signed(terrain.width) ? terrain.map[vertX][vertZ + 1] : vertex;
				float down = signed(vertZ) - 1 >= 0 ? terrain.map[vertX][vertZ - 1] : vertex;

				DirectX::SimpleMath::Vector3 normal = DirectX::SimpleMath::Vector3(left - right, 2.f, down - up);
				normal.Normalize();

				// scale the vector up into the 8-bit range
				normal *= 128;
				normalBuffer.get()[index * 3] = char(normal.x);
				normalBuffer.get()[index * 3 + 1] = char(normal.y);
				normalBuffer.get()[index * 3 + 2] = char(normal.z);
				// update vertex index
				index++;
			}
		}

		//-----------------------------------------------
		// output file stream
		//-----------------------------------------------

		// terrain
		ofstream terrainStream(m_directory / "terrain.dat", std::ios::binary);
		terrainStream.write((const char *)terrainBuffer.get(), vertexCount * sizeof(short));
		terrainStream.close();
		// biome
		ofstream biomeStream(m_directory / "biome.dat", std::ios::binary);
		biomeStream.write((const char *)biomeBuffer.get(), sampleCount * sizeof(float));
		biomeStream.close();
		// NormalMap
		ofstream normal(m_directory / "normal.dat", std::ios::binary);
		normal.write((const char *)normalBuffer.get(), vertexCount * 3);
		normal.close();
	}

	void TerrainSystem::SaveWater(Map<WaterCell> & water)
	{
		// Water Map
		unsigned int vertexCount = (water.width + 1) * (water.width + 1);
		unique_ptr<unsigned char[]> terrainBuffer(new unsigned char[vertexCount * sizeof(short)]);

		unsigned int index = 0;
		unsigned int biomeIndex = 0;

		for (unsigned short vertZ = 0; vertZ <= water.width; vertZ++) {
			for (unsigned short vertX = 0; vertX <= water.width; vertX++) {
				// verticies
				float vertex = water.map[vertX][vertZ].Water;
				short vertexShort = (short)(vertex * 100.f);
				std::memcpy(&terrainBuffer.get()[index * sizeof(short)], &vertexShort, sizeof(short));
				// update vertex index
				index++;
			}
		}

		//-----------------------------------------------
		// output file stream
		//-----------------------------------------------
		ofstream waterStream(m_directory / "water.dat", std::ios::binary);
		waterStream.write((const char *)terrainBuffer.get(), vertexCount * sizeof(short));
		waterStream.close();
	}

	/*void TerrainSystem::CreateResourceEntities()
	{
		Utility::OutputLine("Generating Resources...");
		static const float density = 0.005f;
		for (int x = 0; x <= TerrainMap->width; x++) {
			for (int z = 0; z <= TerrainMap->length; z++) {
				float probability = density;
				probability *= ResourceGradientProbability(TerrainMap->GradientAngle(x, z));

				if (probability > 0.0001f && Utility::Chance(probability)) {
					Vector3 pos(x, TerrainMap->Height(x, z) - 1.f, z);
					Vector3 rot(Utility::randWithin(0.f, XM_2PI), Utility::randWithin(0.f, XM_2PI), Utility::randWithin(0.f, XM_2PI));
					if (Utility::Chance(0.5f)) {
						NewResourceNode(pos, rot, "Iron_Boulder", std::map<string, int>{
							{"Iron Ore", 1}
						});
					}
					else {
						NewResourceNode(pos, rot, "Boulder", std::map<string, int>{
							{"Cut Limestone", 1}
						});
					}
				}
			}
		}
		Utility::OutputLine("Finished");
	}*/

	//void TerrainSystem::NewResourceNode(Vector3 & position, Vector3 & rotation, string model, std::map<string, int> inventoryItems)
	//{

	//	// Fill the tree's inventory with logs
	//	Inventory inventory = Inventory();
	//	auto itemSys = SM->GetSystem<ItemSystem>("Item");
	//	for (auto & item : inventoryItems) {
	//		itemSys->AddItem(inventory, item.first, item.second);
	//	}
	//	Action action = Action(Vector3(3.f, 3.f, 3.f), EventTypes::Resource_Aquire, 0);
	//	action.TargetEntity = EM->CreateEntity<Model, Inventory, Action>(
	//		Position(position, rotation),
	//		Model(0, AssetType::Authored),
	//		std::move(inventory),
	//		std::move(action)
	//	);
	//}

	float TerrainSystem::ResourceGradientProbability(float gradient)
	{
		static const float minGradient = Utility::DegToRad(60);
		return 1.f - Utility::SigmoidDecay(gradient, minGradient);
	}

	void TerrainSystem::CreateTerrainEntities()
	{
		for (int x = 0; x < m_width / m_regionWidth; x++) {
			for (int z = 0; z < m_width / m_regionWidth; z++) {
				Vector3 position(
					((double)x * (double)m_regionWidth) + (double)m_regionWidth * 0.5,
					0.0,
					((double)z * (double)m_regionWidth) + (double)m_regionWidth * 0.5
				);
				NewTerrain(position);
			}
		}
	}

	void TerrainSystem::CreateWaterEntities()
	{
		for (int x = 0; x < m_width / m_regionWidth; x++) {
			for (int z = 0; z < m_width / m_regionWidth; z++) {
				Vector3 position(
					((double)x * (double)m_regionWidth) + (double)m_regionWidth * 0.5,
					0.0,
					((double)z * (double)m_regionWidth) + (double)m_regionWidth * 0.5
				);
				NewWater(position);
			}
		}
	}
	void TerrainSystem::NewWater(DirectX::SimpleMath::Vector3 & position)
	{
		/*EM->CreateEntity<Water, Model>(
			Position(position, SimpleMath::Vector3::Zero),
			Water(),
			Model(0, AssetType::Procedural)
			);*/
		EntityPtr terrainAsset;
		if (AssetManager::Get()->Find(AssetManager::Get()->GetProceduralEM(), "water", terrainAsset)) {
			EM->CreateEntity<Water, Model>(
				Position(position, SimpleMath::Vector3::Zero),
				Water(),
				Model(terrainAsset->ID(), AssetType::Procedural)
				);
		}
	}



	//void TerrainSystem::UpdateRegions(Vector3 center)
	//{

	//	center.y = 0;
	//	UpdateCache(center);
	//	for (int i = 0; i < m_terrainEntities.size(); i++) {
	//		auto terrainEntity = m_terrainEntities[i];
	//		auto waterEntity = m_waterEntities[i];
	//		Vector3 position = terrainEntity->GetComponent<Components::Position>("Position")->Pos;
	//		position.y = 0;
	//		double distance = Vector3::Distance(center, position);

	//		shared_ptr<Components::PositionNormalTextureTangentColorVBO> vbo = terrainEntity->GetComponent<Components::PositionNormalTextureTangentColorVBO>("PositionNormalTextureTangentColorVBO");
	//		shared_ptr<Components::PositionNormalTextureTangentColorVBO> waterVBO = waterEntity->GetComponent<Components::PositionNormalTextureTangentColorVBO>("PositionNormalTextureTangentColorVBO");
	//		// Update the Level Of Detail as a funtion of distance
	//		int lod = LOD(distance, (int)m_regionWidth);
	//		// Only update this VBO if the LOD has changed

	//		if (vbo->LOD != lod) {
	//			vbo->LOD = lod;
	//			waterVBO->LOD = lod;
	//			// Get the region coordinates
	//			int x = (int)std::floor(position.x / (double)(int)m_regionWidth);
	//			int z = (int)std::floor(position.z / (double)(int)m_regionWidth);

	//			TaskManager::Get().Push(Task([=] {
	//				try {
	//					UpdateWaterVBO(waterVBO, UpdateTerrainVBO(vbo, x, z), x, z);
	//				}
	//				catch (std::exception ex) {
	//					Utility::OutputException(ex.what());
	//				}
	//			}));
	//			/*std::thread([this, vbo, waterVBO, x, z]() {


	//			}).detach();*/
	//			//m_worker.join();
	//			//UpdateTerrainVBO(vbo, x, z);

	//		}
	//	}
	//}

	void TerrainSystem::UpdateWaterVBO(shared_ptr<Components::PositionNormalTextureTangentColorVBO> vbo, shared_ptr<HeightMap> terrain, int  regionX, int  regionZ)
	{
		// calculate quad size based off of LOD (Level Of Detail)
		int quadWidth = std::pow(2, vbo->LOD);
		int mapWidth = terrain->width;
		// Load the vertex array with data.
		HeightMap water(terrain->width, 0.0, 0.0, 0);
		ifstream waterStream(m_directory / "water.dat", ios::binary);

		static const float push = -0.2f;
		if (waterStream.is_open()) {
			for (int vertZ = 0; vertZ <= (int)mapWidth; vertZ++) {
				for (int vertX = 0; vertX <= (int)mapWidth; vertX++) {
					int worldX = vertX * quadWidth + (int)m_regionWidth * regionX;
					int worldZ = vertZ * quadWidth + (int)m_regionWidth * regionZ;
					int index = Utility::posToIndex(worldX, worldZ, m_width + 1);
					// heightMap
					water.map[vertX][vertZ] = InternalHeight(waterStream, index, 100.f) + push;

				}
			}

			waterStream.close();
		}
		vbo->Vertices.clear();
		vbo->Indices.clear();

		for (int z = 0; z < mapWidth; z++) {
			for (int x = 0; x < mapWidth; x++) {
				/*
				Note:
				It might be possible to correct unstable erosion processes by limiting the amout of erosion
				to the lowest point in the immediate quad, e.g. do not allow it to dig holes, only push sediment
				off of a ledge.

				*/
				const float worldX = x * quadWidth + regionX * m_regionWidth;
				const float worldZ = z * quadWidth + regionZ * m_regionWidth;
				bool hasWater[4]{
					water.map[x][z] > push,
					water.map[x + 1][z] > push,
					water.map[x][z + 1] > push,
					water.map[x + 1][z + 1] > push,
				};
				Vector3 quad[4]{
					Vector3(
						(float)(worldX),
						terrain->map[x][z] > 0.f ? (water.map[x][z] < 0.f ? LowestNeighbor(water,*terrain, x, z) : water.map[x][z] + terrain->map[x][z]) : 0.f,
						(float)(z * quadWidth + regionZ * m_regionWidth)
					),
					Vector3(
						(float)(worldX + quadWidth),
						terrain->map[x + 1][z] > 0.f ? (water.map[x + 1][z] < 0.f ? LowestNeighbor(water,*terrain, x + 1, z) : water.map[x + 1][z] + terrain->map[x + 1][z]) : 0.f,
						(float)(worldZ)
					),

					Vector3(
						(float)(worldX),
						terrain->map[x][z + 1] > 0.f ? (water.map[x][z + 1] < 0.f ? LowestNeighbor(water,*terrain, x, z + 1) : water.map[x][z + 1] + terrain->map[x][z + 1]) : 0.f,
						(float)(worldZ + quadWidth)
					),
					Vector3(
						(float)(worldX + quadWidth),
						terrain->map[x + 1][z + 1] > 0.f ? (water.map[x + 1][z + 1] < 0.f ? LowestNeighbor(water,*terrain, x + 1, z + 1) : water.map[x + 1][z + 1] + terrain->map[x + 1][z + 1]) : 0.f,
						(float)(worldZ + quadWidth)
					)
				};
				if (x != mapWidth && z != mapWidth) {
					/*
					0---1
					| \ |
					2---3
					*/

					// Left triangle
					if (hasWater[0] || hasWater[3] || hasWater[2]) {
						vbo->Indices.push_back(vbo->Vertices.size());
						vbo->Vertices.push_back(CreateVertex(quad[0], Vector3::UnitY, Vector2::Zero));
						vbo->Indices.push_back(vbo->Vertices.size());
						vbo->Vertices.push_back(CreateVertex(quad[2], Vector3::UnitY, Vector2(0.f, 1.f)));
						vbo->Indices.push_back(vbo->Vertices.size());
						vbo->Vertices.push_back(CreateVertex(quad[3], Vector3::UnitY, Vector2(1.f, 1.f)));
					}
					// Right
					if (hasWater[0] || hasWater[1] || hasWater[3]) {
						vbo->Indices.push_back(vbo->Vertices.size());
						vbo->Vertices.push_back(CreateVertex(quad[0], Vector3::UnitY, Vector2::Zero));
						vbo->Indices.push_back(vbo->Vertices.size());
						vbo->Vertices.push_back(CreateVertex(quad[3], Vector3::UnitY, Vector2(1.f, 1.f)));
						vbo->Indices.push_back(vbo->Vertices.size());
						vbo->Vertices.push_back(CreateVertex(quad[1], Vector3::UnitY, Vector2(1.f, 0.f)));
					}
				}
				//if (x != 0 && z != 0) {
				//	if (terrain->map[x][z - 1] > 0.f) {
				//		float vertex = terrain->map[x][z - 1] + (water.map[x][z - 1] < 0.05 ? pushDepth : 0.f);
				//		// normals
				//		float left = signed(x) - 1 >= 0 ? terrain->map[x - 1][z - 1] + water.map[x - 1][z - 1] : vertex;
				//		float right = signed(x) + 1 <= signed(terrain->width) ? terrain->map[x + 1][z - 1] + water.map[x + 1][z - 1] : vertex;
				//		float up = signed(z - 1) + 1 <= signed(terrain->width) ? terrain->map[x][z] + water.map[x][z] : vertex;
				//		float down = signed(z - 1) - 1 >= 0 ? terrain->map[x][z - 2] + water.map[x][z - 2] : vertex;

				//		DirectX::SimpleMath::Vector3 normal = DirectX::SimpleMath::Vector3(left - right, 2.f, down - up);
				//		normal.Normalize();
				//		vbo->Vertices[vertexBufferIndex - (water.width + 1)].normal = normal;
				//	}
				//	else {
				//		vbo->Vertices[vertexBufferIndex - (water.width + 1)].normal = DirectX::SimpleMath::Vector3::Up;
				//	}
				//}

			}
		}

		vbo->LODchanged = true;
	}

	VertexPositionNormalTangentColorTexture TerrainSystem::CreateVertex(Vector3 position, Vector3 normal, Vector2 texture)
	{
		return VertexPositionNormalTangentColorTexture(position, normal, Vector4::Zero, Vector4::Zero, texture);
	}

	float TerrainSystem::LowestNeighbor(HeightMap & water, HeightMap & terrain, int x, int z)
	{
		float minY = std::numeric_limits<float>::infinity();
		for (int i = 0; i < 8; i++) {
			int adjX = (i == 0 || i == 1 || i == 7 ? x + 1 : (i >= 3 && i <= 5 ? x - 1 : x));
			int adjZ = (i >= 1 && i <= 3 ? z + 1 : (i >= 5 && i <= 7 ? z - 1 : z));
			if (water.Bounded(adjX, adjZ)) {
				float worldY = water.map[adjX][adjZ] + terrain.map[adjX][adjZ];
				if (worldY < minY) minY = worldY;
			}
		}
		return minY;
	}

	shared_ptr<HeightMap> TerrainSystem::UpdateTerrainVBO(shared_ptr<Components::PositionNormalTextureTangentColorVBO> vbo, int  regionX, int regionZ)
	{
		// calculate quad size based off of LOD (Level Of Detail)
		int quadWidth = std::pow(2, vbo->LOD);
		int mapWidth = m_regionWidth / quadWidth;
		// Load the vertex array with data.
		shared_ptr<HeightMap> heightMap = std::make_shared<HeightMap>(mapWidth, 0.0, 0.0, 0);
		Map<Vector3> normalMap(mapWidth, 0.0, 0.0, 0);
		ifstream terrainStream(m_directory / "terrain.dat", ios::binary);
		ifstream normalStream(m_directory / "normal.dat", ios::binary);


		if (terrainStream.is_open() && normalStream.is_open()) {
			// stores the exact bytes from the file into memory
				//char *terrainCharBuffer = new char[regionSize];
			//char *normalCharBuffer = new char[vertexCount * 3];
			// move start position to the region, and proceed to read each line into the Char buffers


			for (int vertZ = 0; vertZ <= (int)mapWidth; vertZ++) {
				for (int vertX = 0; vertX <= (int)mapWidth; vertX++) {
					int worldX = vertX * quadWidth + (int)m_regionWidth * regionX;
					int worldZ = vertZ * quadWidth + (int)m_regionWidth * regionZ;
					int index = Utility::posToIndex(worldX, worldZ, m_width + 1);
					// heightMap
					heightMap->map[vertX][vertZ] = InternalHeight(terrainStream, index, 10.f);
					// normalMap
					normalMap.map[vertX][vertZ] = Normal(normalStream, index);
				}
			}

			terrainStream.close();
			normalStream.close();
		}
		// create 2 triangles (6 vertices) for every quad in the region
		vbo->Vertices.resize(6 * mapWidth * mapWidth);
		vbo->Vertices.shrink_to_fit();
		vbo->Indices.resize(6 * mapWidth * mapWidth);
		vbo->Indices.shrink_to_fit();
		int index = 0;
		for (int z = 0; z < mapWidth; z++) {
			for (int x = 0; x < mapWidth; x++) {
				// Get the indexes to the four points of the quad.

				// Upper left.
				Vector3 vertex1(
					(float)(x * quadWidth + regionX * m_regionWidth),
					heightMap->map[x][z],
					(float)(z * quadWidth + regionZ * m_regionWidth));
				// Upper right.
				Vector3 vertex2(
					(float)((x + 1) * quadWidth + regionX * m_regionWidth),
					heightMap->map[x + 1][z],
					(float)(z * quadWidth + regionZ * m_regionWidth));
				// Bottom left.
				Vector3 vertex3(
					(float)(x * quadWidth + regionX * m_regionWidth),
					heightMap->map[x][z + 1],
					(float)((z + 1) * quadWidth + regionZ * m_regionWidth));
				// Bottom right.
				Vector3 vertex4(
					(float)((x + 1) * quadWidth + regionX * m_regionWidth),
					heightMap->map[x + 1][z + 1],
					(float)((z + 1) * quadWidth + regionZ * m_regionWidth));

				/*
				1---2
				| \ |
				3---4
				*/

				// Triangle 1 - Upper left
				vbo->Vertices[index] = CreateVertex(
					XMFLOAT3(vertex1),				// position
					XMFLOAT3(normalMap.map[x][z]),	// normal
					XMFLOAT2(0.f, 0.f)				// texture
				);
				vbo->Indices[index] = index;
				index++;
				// Triangle 1 - Bottom right.
				vbo->Vertices[index] = CreateVertex(
					XMFLOAT3(vertex4),										// position
					XMFLOAT3(normalMap.map[x + 1][z + 1]),	// normal
					XMFLOAT2(1.f, 1.f)										// texture
				);
				vbo->Indices[index] = index;
				index++;
				// Triangle 1 - Bottom left.
				vbo->Vertices[index] = CreateVertex(
					XMFLOAT3(vertex3),											// position
					XMFLOAT3(normalMap.map[x][z + 1]),	// normal
					XMFLOAT2(0.f, 1.f)											// texture
				);
				vbo->Indices[index] = index;
				index++;
				// Triangle 2 - Upper left.
				vbo->Vertices[index] = CreateVertex(
					XMFLOAT3(vertex1),										// position
					XMFLOAT3(normalMap.map[x][z]),	// normal
					XMFLOAT2(0.f, 0.f)										// texture
				);
				vbo->Indices[index] = index;
				index++;
				// Triangle 2 - Upper right.
				vbo->Vertices[index] = CreateVertex(
					XMFLOAT3(vertex2),											// position
					XMFLOAT3(normalMap.map[x + 1][z]),	// normal
					XMFLOAT2(1.f, 0.f)											// texture
				);
				vbo->Indices[index] = index;
				index++;
				// Triangle 2 - Bottom right.
				vbo->Vertices[index] = CreateVertex(
					XMFLOAT3(vertex4),												// position
					XMFLOAT3(normalMap.map[x + 1][z + 1]),	// normal
					XMFLOAT2(1.f, 1.f)												// texture
				);
				vbo->Indices[index] = index;
				index++;
			}
		}

		vbo->LODchanged = true;
		return heightMap;
	}

	int TerrainSystem::LOD(double distance, unsigned int modelWidth)
	{
		// add integer to decrease terrain detail
		return 0 + std::min((int)std::log2(modelWidth), std::max(0, (int)std::floor(std::log2(distance / (double)modelWidth))));
	}

	void TerrainSystem::UpdateCache(Vector3 center)
	{

		static const int gridSize = 64;
		// sample area
		Rectangle sampleArea = Rectangle(std::floor(center.x / gridSize) * gridSize, std::floor(center.z / gridSize) * gridSize, gridSize, gridSize);
		m_cache = AssetManager::Get()->GetHeightMap("terrain", AssetType::Procedural, sampleArea);
	}

	void TerrainSystem::NewTerrain(DirectX::SimpleMath::Vector3 & position)
	{
		EntityPtr terrainAsset;
		if (AssetManager::Get()->Find(AssetManager::Get()->GetProceduralEM(), "terrain", terrainAsset)) {
			EM->CreateEntity<Terrain, Model>(
				Position(position, SimpleMath::Vector3::Zero),
				Terrain(),
				Model(terrainAsset->ID(), AssetType::Procedural)
				);
		}
		//Components::PositionNormalTextureTangentColorVBO * vbo = new Components::PositionNormalTextureTangentColorVBO();
		//vbo->Effect = "Terrain";
		//entity->AddComponent(vbo);
	}

	Vector3 TerrainSystem::Normal(std::ifstream & ifs, const int & index)
	{

		char normalBuffer[3];
		ifs.seekg(index * 3 * sizeof(char));
		ifs.read((char *)normalBuffer, 3 * sizeof(char));
		Vector3 normal = Vector3(float(normalBuffer[0]), float(normalBuffer[1]), float(normalBuffer[2]));
		normal.Normalize();
		return normal;
	}

	void TerrainSystem::ImportMap(HeightMap & map)
	{

		/*ifstream terrainStream(m_directory / "terrain.dat", ios::binary);
		for (int i = 0; i <=  map.width; i++) {
			for (int j = 0; j <= map.length; j++) {
				map.map[i][j] = InternalHeight(terrainStream, Utility::posToIndex(map.area.x + i, map.area.y + j, m_width + 1), 10.f);
			}
		}*/
	}

	float TerrainSystem::Average(HeightMap & map, Rectangle area)
	{
		float sum = 0.f;
		for (int x = area.x; x <= area.x + area.width; x++) {
			for (int z = area.y; z <= area.y + area.height; z++) {
				sum += map.ValueAt(x, z);
			}
		}
		return sum / ((area.width + 1) * (area.height + 1));
	}

	void TerrainSystem::Save(HeightMap & map)
	{
		std::fstream terrainStream(m_directory / "terrain.dat", ios::ate | ios::out | ios::in | std::ios_base::binary);
		for (int i = map.area.x; i <= map.area.x + map.area.width; i++) {
			for (int j = map.area.y; j <= map.area.y + map.area.height; j++) {
				short vertexShort = (short)(map.ValueAt(i, j) * 10.f);
				char * buffer = new char[sizeof(short)];
				std::memcpy(buffer, &vertexShort, sizeof(short));
				terrainStream.seekp(Utility::posToIndex(i, j, m_width + 1) * sizeof(short));
				terrainStream.write(buffer, sizeof(short));
				delete[] buffer;
			}
		}
		terrainStream.close();
	}

	void TerrainSystem::RegisterHandlers()
	{
		IEventManager::RegisterHandler(EventTypes::Movement_PlayerMoved, std::function<void(float, float)>([&](float x, float z) {
			UpdateCache(Vector3(x,0.f,z));
		}));
	}

	float TerrainSystem::Flatten(HeightMap & cache, Rectangle area, const int margin)
	{
		Rectangle footprint = Rectangle(area.x - margin, area.y - margin, area.width + 2 * margin, area.height + 2 * margin);

		float targetHeight = Average(cache, footprint);
		for (int x = footprint.x; x <= footprint.x + footprint.width;x++) {
			for (int z = footprint.y; z <= footprint.y + footprint.height;z++) {
				if (x == footprint.x + footprint.width - margin && z == footprint.y + footprint.height) {
					auto test = 9;
				}
				Vector2 location = Vector2(x, z);
				if (area.Contains(location)) {
					cache.ValueAt(location.x, location.y) = targetHeight;
				}
				else {
					Vector2 inner = geometry::Clamp(area, location);

					Vector2 tangent = location - inner;
					float t = tangent.Length();
					if (t <= margin) {
						tangent.Normalize();
						Vector2 outer = inner + tangent * margin;

						cache.ValueAt(location.x, location.y) = Utility::Lerp(targetHeight, cache.HeightAbsPos(outer), t / float(margin));

					}
					/*if (location.x < area.x) {
						cache.ValueAt(location.x,location.y) = Utility::Lerp(targetHeight, cache.ValueAt(area.x - margin,location.y), float(area.x - location.x)/(float)margin);
					}
					else if (location.x > area.x + area.width) {
						cache.ValueAt(location.x, location.y) = Utility::Lerp(targetHeight, cache.ValueAt(area.x + area.width + margin,location.y), float(location.x-(area.x + area.width)) / (float)margin);
					} else {

					}

					if (location.y < area.y) {
						cache.ValueAt(location.x, location.y) = Utility::Lerp(targetHeight, cache.ValueAt(location.x, area.y - margin), float(area.y - location.y) / (float)margin);
					}
					else if (location.y > area.y + area.height) {
						cache.ValueAt(location.x, location.y) = Utility::Lerp(targetHeight, cache.ValueAt(location.x, area.y + area.height + margin), float(location.y - (area.y + area.height)) / (float)margin);
					}*/
				}
			}
		}
		return Utility::Floor(targetHeight, 0.1f);
	}

	float TerrainSystem::Height(HeightMap & map, int x, int z)
	{
		return 0.0f;
	}

	float TerrainSystem::InternalHeight(std::ifstream & ifs, const int & index, float precision)
	{
		// heightMap
		char shortBuffer[2];
		ifs.seekg(index * sizeof(short));
		ifs.read((char *)&shortBuffer, sizeof(short));
		/*char lower = (char)shortBuffer[0];
		short upper = (short)shortBuffer[1] << 8;*/
		//short shortVertex = (short)(lower | upper);
		short shortVertex = 0;
		memcpy(&shortVertex, &shortBuffer, sizeof(short));
		return (float)shortVertex / precision;
	}

	float TerrainSystem::Diamond(HeightMap & map, int & x, int & y, int & distance) {
		float sum = 0.f;
		sum += map.map[x - distance][y - distance];
		sum += map.map[x - distance][y + distance];
		sum += map.map[x + distance][y - distance];
		sum += map.map[x + distance][y + distance];
		return sum * 0.25f;
	}
	float TerrainSystem::Square(HeightMap & map, int & x, int & y, int & distance) {
		float sum = 0;
		int denominator = 0;
		if (y > 0) {
			sum += map.map[x][y - distance];
			denominator++;
		}
		if (y < map.width - 1) {
			sum += map.map[x][y + distance];
			denominator++;
		}
		if (x > 0) {
			sum += map.map[x - distance][y];
			denominator++;
		}
		if (x < map.width - 1) {
			sum += map.map[x + distance][y];
			denominator++;
		}
		return sum / denominator;
	}
	float TerrainSystem::Deviation(float range, float offset) {
		return Utility::randWithin(-range * 0.5f + offset, range * 0.5f + offset);
	}
	float TerrainSystem::BiomeDeviation(float biome, float continent) {
		return Utility::Gaussian(biome, m_biomeAmplitude, m_biomeShift, m_biomeWidth) * Utility::Sigmoid(continent, 1.f, m_continentShift, m_continentWidth);
	}

	void TerrainSystem::InitializeErosionMap()
	{
		WaterMap = std::make_unique<Map<WaterCell>>(m_width, 0.f, 0.f, 0);

		//// Initialize the water entity
		//waterEntity = EM->NewEntity();
		//waterEntity->AddComponent(EM->ComponentMask("VBO"), std::shared_ptr<Components::Component>(
		//	new Components::VBO(waterEntity->ID())));
		//shared_ptr<Components::VBO> vbo = EM->GetComponent<Components::VBO>(waterEntity, "VBO");
		//vbo->Effect = "Water";
		//// create 2 triangles (6 vertices) for every quad in the world
		//vbo->Vertices.resize((m_width +1)* (m_width+1));
		//vbo->Vertices.shrink_to_fit();
		//vbo->Indices.resize(6 * m_width * m_width);
		//vbo->Indices.shrink_to_fit();
	}

	shared_ptr<Map<ThermalCell>> TerrainSystem::InitializeThermalErosionMap()
	{
		return std::make_shared<Map<ThermalCell>>(m_width, 0.f, 0.f, 0);
	}

	void TerrainSystem::UpdateWater(HeightMap & terrain, Map<WaterCell> & water, std::function<void(float)> && progressCallback) {

		const int iterations = 150;
		const float rainPortion = 0.3f;

		for (int i = 0; i < iterations * 2; i++) {
			for (int z = 0; z <= m_width; z++) {
				for (int x = 0; x <= m_width; x++) {
					WaterCell & thisCell = water.map[x][z];
					if (terrain.map[x][z] > 0.f) {
						if (i % 2 == 0) {
							thisCell.UpdateFlux(terrain, water, x, z);
						}
						else {
							thisCell.UpdateWater(terrain, water, x, z);
							if ((float)i / (float)(iterations * 2) <= rainPortion) thisCell.Rain();
							thisCell.Evaporate();
						}
					}
					else if (i == 0 && terrain.map[x][z] <= 0.f) {
						thisCell.Water = -terrain.map[x][z];

					}
					// bandaid
					if (std::abs(thisCell.Water) > 1000) {
						thisCell.Water = 0;
					}
				}
			}
			progressCallback((float)i / (float)(iterations * 2.f));
			OutputDebugStringA(("\nWater Sim @ " + std::to_string((float)i / (float)(iterations * 2.f) * 100.f) + "%").c_str());
		}
		for (int i = 0; i < 0; i++) {
			for (int z = 0; z <= m_width; z++) {
				for (int x = 0; x <= m_width; x++) {
					WaterCell & thisCell = water.map[x][z];
					thisCell.Evaporate();
				}
			}
		}
	}
	void TerrainSystem::UpdateDroplets(HeightMap & terrain, shared_ptr<vector<Droplet>> droplets, shared_ptr<Map<ThermalCell>> thermal, std::function<void(float)> && progressCallback)
	{
		static const int iterations = 300;
		static const int thermalPeriod = iterations / 20;
		static const int thermalCutoff = 100;
		for (int i = 0; i < iterations; i++) {
			// calculate changes for each drop
			if (i > thermalCutoff) {
				for (Droplet & droplet : *droplets) {
					droplet.Update(terrain);
				}
			}
			if (i < thermalCutoff || i % thermalPeriod == 0) {
				// Thermal erosion
				for (int thermalI = 0; thermalI < 2; thermalI++) {
					for (int x = 0; x <= m_width; x++) {
						for (int z = 0; z <= m_width; z++) {
							if (terrain.map[x][z] >= -100.f) {
								if (thermalI % 2 == 0) {
									thermal->map[x][z].UpdateFlux(terrain, *thermal.get(), x, z);
								}
								else {
									thermal->map[x][z].TransferSediment(terrain, *thermal.get(), x, z);
								}
							}
						}
					}
				}
			}
			progressCallback((float)i / (float)iterations);
			OutputDebugStringA(("\nErosion Sim @ " + std::to_string(((float)i / (float)iterations)* 100.f) + "%").c_str());
		}
	}
	shared_ptr<vector<Droplet>> TerrainSystem::InitializeDroplets()
	{
		const int dropletCount = (int)(0.071f * (float)(m_width * m_width));
		auto droplets = std::make_shared<vector<Droplet>>();
		for (int i = 0; i < dropletCount; i++) {
			Droplet droplet;
			droplet.Reset(m_width);
			droplets->push_back(droplet);
		}
		return droplets;
	}
}