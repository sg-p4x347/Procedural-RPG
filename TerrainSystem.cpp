#include "pch.h"
#include "TerrainSystem.h"
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
#include "BuildingSystem.h"
#include "TaskManager.h"
#include "Inventory.h"
#include "IEventManager.h"
#include "Movement.h"
#include "Global.h"

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
		m_regionWidth(regionWidth),
		m_chunks(worldWidth / regionWidth),
		m_normals(worldWidth / regionWidth),
		m_chunkModels(worldWidth / regionWidth),
		m_chunkLOD(worldWidth / regionWidth)
	{
		m_directory = directory / Name();
		Filesystem::create_directory(m_directory);
		RegisterHandlers();
		// initialize chunk lods to an unplausible Low lod
		for (int x = 0; x < m_chunkLOD.width;x++) {
			for (int z = 0; z < m_chunkLOD.length;z++) {
				m_chunkLOD[x][z] = 100;
				m_chunks[x][z] = std::make_shared<HeightMap>();
			}
		}

	}


	TerrainSystem::~TerrainSystem()
	{
	}

	shared_ptr<DirectX::Model> TerrainSystem::GetModel(shared_ptr<world::WEM::RegionType> region)
	{
		int x = region->GetArea().x / m_regionWidth;
		int z = region->GetArea().y / m_regionWidth;
		if (m_mutexRegion.first == x && m_mutexRegion.second == z && m_writingModel) {
			return m_chunkBuffer;
		}
		else {
			return m_chunkModels[x][z];
		}
	}

	void TerrainSystem::Generate(std::function<void(float,std::string)> && progressCallback)
	{
		JsonParser config(std::ifstream("config/continent.json"));
		// Mountain settings
		JsonParser mountains = config["mountains"];
		m_biomeAmplitude = mountains["biomeAmplitude"].To<float>();
		m_biomeShift = mountains["biomeShift"].To<float>();
		m_biomeWidth = mountains["biomeWidth"].To<float>();
		m_continentShift = mountains["continentShift"].To<float>();
		m_continentWidth = mountains["continentWidth"].To<float>();
		// General
		m_sampleSpacing = config["sampleSpacing"].To<int>();
		const float landAmplitude = config["landAmplitude"].To<float>();
		const float oceanAmplitude = config["oceanAmplitude"].To<float>();
		const float biomeDeviationConst = config["biomeDeviationConst"].To<float>();

		// Terrain map
		JsonParser terrainMap = config["terrainMap"];
		m_terrainMap.reset(new HeightMap(m_width, m_width * terrainMap["initialDeviation"].To<double>(), terrainMap["deviationDecrease"].To<double>(), 0));
		// Continent map
		JsonParser continentMap = config["continentMap"];
		const float CM_offset = continentMap["offset"].To<double>();
		HeightMap continent = HeightMap(m_terrainMap->width / m_sampleSpacing, (m_terrainMap->width / m_sampleSpacing) * continentMap["initialDeviation"].To<double>(), continentMap["deviationDecrease"].To<double>(), continentMap["zoom"].To<int>());
		// Biome map
		JsonParser biomeMap = config["biomeMap"];
		HeightMap biome = HeightMap(m_terrainMap->width / m_sampleSpacing, biomeMap["initialDeviation"].To<double>(), biomeMap["deviationDecrease"].To<double>(), biomeMap["zoom"].To<int>());

		//Load(); TEMP
		// initialize the corners
		for (int x = 0; x <= continent.width; x += continent.width) {
			for (int y = 0; y <= continent.width; y += continent.width) {
				continent.map[x][y] = -continent.initialDeviation;
				biome.map[x][y] = Deviation(biome.initialDeviation);
				float continentZ = (continent.map[x][y] + (CM_offset * continent.initialDeviation)) * m_sampleSpacing;
				m_terrainMap->map[x*m_sampleSpacing][y*m_sampleSpacing] = continentZ * oceanAmplitude + BiomeDeviation(biome.map[x][y], continentZ);
			}
		}
		//auto start = std::chrono::high_resolution_clock::now();
		//auto end = std::chrono::high_resolution_clock::now();
		//auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		// iterate
		for (int iteration = 0; iteration < floor(log2(m_terrainMap->width)); iteration++) {
			int gridWidth = (m_terrainMap->width / pow(2, iteration)) / 2;
			// decrease deviation for the decrease in distance between points
			if (iteration >= continent.zoom) {
				continent.squareDeviation = continent.diamondDeviation * 1.4f;//0.707106781f;
			}
			if (iteration >= biome.zoom) {
				biome.squareDeviation = biome.diamondDeviation * 1.4f;//0.707106781f;
			}
			m_terrainMap->squareDeviation = m_terrainMap->diamondDeviation * 1.4f;//0.707106781f;
			for (int x = gridWidth; x < m_terrainMap->width; x += gridWidth * 2) {
				for (int y = gridWidth; y < m_terrainMap->width; y += gridWidth * 2) {
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
						m_terrainMap->map[x][y] = continentSample;
						m_terrainMap->map[x][y] += biomeDeviation;
						if (iteration > 2) {
							m_terrainMap->map[x][y] += (m_terrainMap->diamondDeviation * Deviation(biomeDeviation)*biomeDeviationConst);
						}
					}
					else {
						m_terrainMap->map[x][y] = Diamond(*m_terrainMap, x, y, gridWidth);
						m_terrainMap->map[x][y] += Deviation(m_terrainMap->diamondDeviation);
						m_terrainMap->map[x][y] += (m_terrainMap->diamondDeviation * Deviation(biomeDeviation)*biomeDeviationConst);
					}
					//-----------
					// Square
					//-----------
					for (float rad = (x == m_terrainMap->width - gridWidth || y == m_terrainMap->width - gridWidth ? 0.f : 2.f); rad < 4.f; rad++) {
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
							m_terrainMap->map[pointX][pointY] = continentSquareSample;
							m_terrainMap->map[pointX][pointY] += biomeDeviation;
							if (iteration > 2) {
								m_terrainMap->map[pointX][pointY] += (m_terrainMap->squareDeviation *Deviation(biomeDeviation)*biomeDeviationConst);
							}
						}
						else {
							m_terrainMap->map[pointX][pointY] = Square(*m_terrainMap, pointX, pointY, gridWidth);
							m_terrainMap->map[pointX][pointY] += Deviation(m_terrainMap->squareDeviation);
							m_terrainMap->map[pointX][pointY] += (m_terrainMap->squareDeviation *Deviation(biomeDeviation)*biomeDeviationConst);
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
			m_terrainMap->diamondDeviation *= (0.5f * m_terrainMap->deviationDecrease);
		}

		// Erosion
		bool erosion = config["erosion"].To<bool>();
		if (erosion) UpdateDroplets(*m_terrainMap, InitializeDroplets(), InitializeThermalErosionMap(), [=](float percent) {
			progressCallback(percent * 0.5f, "Erosion Simulation ...");
		});
		// Terrain done yay!
		//AssetManager::Get()->CreateHeightMapModel("terrain", TerrainMap.get(), AssetManager::Get()->CreateNormalMap(TerrainMap.get()), 10.f, m_regionWidth, "Terrain");


		// Rain simulation
		InitializeErosionMap();
		if (erosion) UpdateWater(*m_terrainMap, *m_waterMap, [=](float percent) {
			progressCallback(0.5f + percent * 0.5f, "Water Simulation ...");
		});
		// Water done yay!
		
		// package everything into a unified terrain vertex
		Map<TerrainVertex> vertices(m_width);
		for (int x = 0; x < m_waterMap->width;x++) {
			for (int z = 0; z < m_waterMap->length;z++) {
				TerrainVertex vertex;
				vertex.terrain = (*m_terrainMap)[x][z];
				vertex.water = (*m_waterMap)[x][z].Water;
				{
					float center = vertex.terrain;
					// adjacent vertices, if on the edge, this vertex is used
					float left = x - 1 >= 0 ? m_terrainMap->Height(x - 1, z) : center;
					float right = x + 1 <= m_width ? m_terrainMap->Height(x + 1, z) : center;
					float up = z + 1 <= m_width ? m_terrainMap->Height(x, z + 1) : center;
					float down = z - 1 >= 0 ? m_terrainMap->Height(x, z - 1) : center;

					Vector3 normal = DirectX::SimpleMath::Vector3(left - right, 2.f, down - up);
					normal.Normalize();
					vertex.terrainNormal = normal;
				}
				{
					float center = vertex.water;
					// adjacent vertices, if on the edge, this vertex is used
					float left = x - 1 >= 0 ? (*m_waterMap)[x - 1][z].Water : center;
					float right = x + 1 <= m_width ? (*m_waterMap)[x + 1][ z].Water : center;
					float up = z + 1 <= m_width ? (*m_waterMap)[x][ z + 1].Water : center;
					float down = z - 1 >= 0 ? (*m_waterMap)[x][z - 1].Water : center;

					Vector3 normal = DirectX::SimpleMath::Vector3(left - right, 2.f, down - up);
					normal.Normalize();
					vertex.waterNormal = normal;
				}
			}
		}
		AssetManager::Get()->CreateMap<TerrainVertex>("terrain", vertices);
		/*AssetManager::Get()->CreateHeightMapModel("water", waterMap.get(), AssetManager::Get()->CreateNormalMap(waterMap->width, waterMap->length, [&](int x, int y) {
			return waterMap->map[x][y] + TerrainMap->map[x][y];
		}), 10.f, m_regionWidth, "Water");*/

		auto position = Vector3(m_width / 2, m_terrainMap->Height(m_width / 2, m_width / 2), m_width / 2);
		EM->GetEntity<Position>(EM->PlayerID())->Get<Position>().Pos = position;
		EM->UpdatePosition(EM->PlayerID(), position);
		// Resources
		//CreateResourceEntities();
		AssetManager::Get()->GetProceduralEM()->Save();
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
		return Height(Vector2((float)x, (float)z));
	}
	void TerrainSystem::SetHeight(const int x, const int z, float value)
	{
		int regionX = x / m_regionWidth;
		int regionZ = z / m_regionWidth;
		if (regionX >= 0 && regionZ >= 0 && regionX < m_width / m_regionWidth && regionZ < m_width / m_regionWidth) {
			return m_chunks[regionX][regionZ]->SetHeight(x - regionX * m_regionWidth, z - regionZ * m_regionWidth,value);
		}
	}
	float TerrainSystem::Height(float & x, float & z)
	{
		return Height(Vector2 (x, z));
		
	}

	float TerrainSystem::Height(Vector2 position)
	{
		int x = position.x / m_regionWidth;
		int z = position.y / m_regionWidth;
		if (x >= 0 && z >= 0 && x < m_width / m_regionWidth && z < m_width / m_regionWidth) {
			return m_chunks[x][z]->Height(position.x - x * m_regionWidth, position.y - z * m_regionWidth);
		}
		return 0.f;
	}

	int TerrainSystem::Width()
	{
		return m_width;
	}

	DirectX::SimpleMath::Rectangle TerrainSystem::Area()
	{
		return DirectX::SimpleMath::Rectangle(0, 0, Width(), Width());
	}

	float TerrainSystem::ResourceGradientProbability(float gradient)
	{
		static const float minGradient = Utility::DegToRad(60);
		return 1.f - Utility::SigmoidDecay(gradient, minGradient);
	}

	void TerrainSystem::Update(double & elapsed)
	{
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

	int TerrainSystem::LOD(double distance, unsigned int modelWidth)
	{
		// add integer to decrease terrain detail
		return 0 + std::min((int)std::log2(modelWidth), std::max(0, (int)std::floor(std::log2(distance / (double)modelWidth))));
	}

	void TerrainSystem::UpdateLOD(Vector3 center)
	{
		std::thread([=] {
			std::lock_guard<std::mutex> guard(m_updateLodMutex);
			int vertexCount = 0;
			const int chunkCount = m_width / m_regionWidth;
			//----------------------------------------------------------------
			// Get fresh LOD indices
			Map<int> lods(chunkCount);
			Map<bool> lodChanged(chunkCount);
			for (int x = 0; x < chunkCount; x++) {
				for (int z = 0; z < chunkCount; z++) {
					Vector2 chunkCenter(x * m_regionWidth + 0.5f * m_regionWidth, z * m_regionWidth + 0.5f * m_regionWidth);
					int lod = LOD(Vector2::Distance(chunkCenter, Vector2(center.x, center.z)), m_regionWidth);
					lods[x][z] = lod;
					lodChanged[x][z] = lod != m_chunkLOD[x][z];
				}
			}
			//----------------------------------------------------------------
			// Stream more detail for higher LOD, downsize for lower LOD
			for (int x = 0; x < chunkCount; x++) {
				for (int z = 0; z < chunkCount; z++) {
					vertexCount += std::pow(m_regionWidth / std::pow(2, lods[x][z]), 2);
					if (m_chunkLOD[x][z] > lods[x][z]) {
						// stream new vertices from the AssetManager
						unsigned int sampleSpacing = std::pow(2, lods[x][z]);
						Rectangle sampleArea = Rectangle(x * m_regionWidth, z * m_regionWidth, m_regionWidth,m_regionWidth);
						m_chunks[x][z] = AssetManager::Get()->GetHeightMap("terrain", AssetType::Procedural, sampleArea, sampleSpacing);
						m_normals[x][z] = AssetManager::Get()->GetNormalMap(m_width, "terrain_normal", AssetType::Procedural, sampleArea, sampleSpacing);

						
					}
					else if (m_chunkLOD[x][z] < lods[x][z]) {
						auto & chunk = *m_chunks[x][z];
						// move verticies into compacted space
						for (int vx = 0; vx <= chunk.width; vx++) {
							for (int vz = 0; vz <= chunk.length; vz++) {
								chunk[vx][vz] = chunk[vx * 2][vz * 2];
							}
						}
						chunk.Resize(m_regionWidth / std::pow(2, lods[x][z]));
					}
				}
			}
			IEventManager::Invoke(EventTypes::GUI_DebugInfo, string("Terrain Vertices:"), to_string(vertexCount));
			for (int x = 0; x < chunkCount; x++) {
				for (int z = 0; z < chunkCount; z++) {
					auto & chunk = *m_chunks[x][z];
					static const Vector2 cardinal[4]{
						Vector2(1.f,0.f),
						Vector2(0.f,1.f),
						Vector2(-1.f,0.f),
						Vector2(0.f,-1.f)
					};
					Vector2 chunkRadius(chunk.width, chunk.length);
					for (int i = 0; i < 4; i++) {
						Vector2 direction = cardinal[i];
						int adjX = x + (int)direction.x;
						int adjZ = z + (int)direction.y;
						if (m_chunks.Bounded(adjX, adjZ)) {
							HeightMap & adjacent = *m_chunks[adjX][adjZ];
							if ((lodChanged[x][z] || lodChanged[adjX][adjZ]) && lods[x][z] < lods[adjX][adjZ]) {
								lodChanged[x][z] = true;
								// Assume that the difference in LOD is a factor of 2 spacing
								if (i % 2 == 0) {
									int vx = i == 0 ? adjacent.width : 0;
									for (int vz = 0; vz < adjacent.length; vz++) {
										chunk[vx * 2][vz * 2 + 1] = (adjacent[adjacent.width - vx][vz] + adjacent[adjacent.width - vx][vz + 1]) * 0.5f;
									}
								}
								else {
									int vz = i == 1 ? adjacent.length : 0;
									for (int vx = 0; vx < adjacent.width; vx++) {
										chunk[vx * 2 + 1][vz * 2] = (adjacent[vx][adjacent.length - vz] + adjacent[vx + 1][adjacent.length - vz]) * 0.5f;
									}
								}
							}
						}
					}
				}
			}
			for (int x = 0; x < chunkCount; x++) {
				for (int z = 0; z < chunkCount; z++) {
					if (lodChanged[x][z]) {
						unsigned int sampleSpacing = std::pow(2, lods[x][z]);
						Rectangle sampleArea = Rectangle(x * m_regionWidth, z * m_regionWidth, m_regionWidth, m_regionWidth);
						shared_ptr<IEffect> effect;
						AssetManager::Get()->GetEffect("Terrain", effect);
						auto model = AssetManager::Get()->CreateTerrainModel(m_width, m_regionWidth, m_chunks[x][z].get(), m_normals[x][z], sampleArea, sampleSpacing, effect);
						m_chunkBuffer = m_chunkModels[x][z];
						m_mutexRegion.first = x;
						m_mutexRegion.second = z;
						m_writingModel = true;
						m_chunkModels[x][z] = model;
						m_writingModel = false;

					}
				}
			}
			
		}).detach();
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

	float TerrainSystem::Average(Rectangle area)
	{
		float sum = 0.f;
		for (int x = area.x; x <= area.x + area.width; x++) {
			for (int z = area.y; z <= area.y + area.height; z++) {
				sum += Height(x, z);
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
		IEventManager::RegisterHandler(EventTypes::WEM_RegionLoaded, std::function<void(shared_ptr<WEM::RegionType>)>([&](shared_ptr<WEM::RegionType> region) {
			// Load terrain from disk into cache
		}));
		IEventManager::RegisterHandler(EventTypes::WEM_RegionUnloaded, std::function<void(shared_ptr<WEM::RegionType>)>([&](shared_ptr<WEM::RegionType> region) {
			// Save cache to disk
		}));
		IEventManager::RegisterHandler(EventTypes::Movement_PlayerMoved, std::function<void(float, float)>([=](float x, float z) {
			UpdateLOD(Vector3(x, 0.f, z));
		}));
	}

	float TerrainSystem::Flatten(Rectangle area, const int margin)
	{
		Rectangle footprint = Rectangle(area.x - margin, area.y - margin, area.width + 2 * margin, area.height + 2 * margin);

		float targetHeight = Average(footprint);
		for (int x = footprint.x; x <= footprint.x + footprint.width;x++) {
			for (int z = footprint.y; z <= footprint.y + footprint.height;z++) {
				Vector2 location = Vector2(x, z);
				if (area.Contains(location)) {
					SetHeight(location.x, location.y, targetHeight);
				}
				else {
					Vector2 inner = geometry::Clamp(area, location);

					Vector2 tangent = location - inner;
					float t = tangent.Length();
					if (t <= margin) {
						tangent.Normalize();
						Vector2 outer = inner + tangent * (float)margin;

						SetHeight(location.x, location.y,Utility::Lerp(targetHeight, Height(outer), t / float(margin)));

					}
				}
			}
		}
		return Utility::Floor(targetHeight, 0.1f);
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
		m_waterMap = std::make_unique<Map<WaterCell>>(m_width, 0.f, 0.f, 0);
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
		static const int iterations = 600;
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