#include "pch.h"
#include "TerrainSystem.h"
#include "Terrain.h"
#include "Position.h"
#include "VBO.h"
#include "Movement.h"
#include "Utility.h"
#include "TaskThread.h"
#include "TerrainCell.h"
#include <thread>
using namespace DirectX::SimpleMath;
static shared_ptr<Entity> waterEntity;
TerrainSystem::TerrainSystem(
	shared_ptr<EntityManager> & entityManager, 
	vector<string> & components, 
	unsigned short updatePeriod, 
	int regionWidth,
	Filesystem::path directory
)
	: System(entityManager,components,updatePeriod), 
	m_regionWidth(regionWidth)
{
	m_directory = directory / Name();
	Filesystem::create_directory(m_directory);
	JsonParser terrainMap = JsonParser(std::ifstream("config/continent.json"))["terrainMap"];
	m_width = terrainMap["width"].To<int>();
	//m_workerThread = TaskThread()

	
}


TerrainSystem::~TerrainSystem()
{
}

void TerrainSystem::Generate()
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
	m_terrain = HeightMap<float>(m_width, m_width * terrainMap["initialDeviation"].To<double>(), terrainMap["deviationDecrease"].To<double>(), 0);
	// Continent map
	JsonParser continentMap = config["continentMap"];
	const float CM_offset = continentMap["offset"].To<double>();
	HeightMap<float> continent = HeightMap<float>(m_terrain.width / m_sampleSpacing, (m_terrain.width / m_sampleSpacing) * continentMap["initialDeviation"].To<double>(), continentMap["deviationDecrease"].To<double>(), continentMap["zoom"].To<int>());
	// Biome map
	JsonParser biomeMap = config["biomeMap"];
	m_biome = HeightMap<float>(m_terrain.width / m_sampleSpacing, biomeMap["initialDeviation"].To<double>(), biomeMap["deviationDecrease"].To<double>(), biomeMap["zoom"].To<int>());

	//Load(); TEMP
	// initialize the corners
	for (int x = 0; x <= continent.width; x += continent.width) {
		for (int y = 0; y <= continent.width; y += continent.width) {
			continent.Map[x][y] = -continent.initialDeviation;
			m_biome.Map[x][y] = Deviation(m_biome.initialDeviation);
			float continentZ = (continent.Map[x][y] + (CM_offset * continent.initialDeviation)) * m_sampleSpacing;
			m_terrain.Map[x*m_sampleSpacing][y*m_sampleSpacing] = continentZ * oceanAmplitude + BiomeDeviation(m_biome.Map[x][y], continentZ);
		}
	}
	//auto start = std::chrono::high_resolution_clock::now();
	//auto end = std::chrono::high_resolution_clock::now();
	//auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	// iterate
	for (int iteration = 0; iteration < floor(log2(m_terrain.width)); iteration++) {
		int gridWidth = (m_terrain.width / pow(2, iteration)) / 2;
		// decrease deviation for the decrease in distance between points
		if (iteration >= continent.zoom) {
			continent.squareDeviation = continent.diamondDeviation * 0.707106781f;
		}
		if (iteration >= m_biome.zoom) {
			m_biome.squareDeviation = m_biome.diamondDeviation * 0.707106781f;
		}
		m_terrain.squareDeviation = m_terrain.diamondDeviation * 0.707106781f;
		for (int x = gridWidth; x < m_terrain.width; x += gridWidth * 2) {
			for (int y = gridWidth; y < m_terrain.width; y += gridWidth * 2) {
				//-----------
				// Diamond
				//-----------
				int sampleX = x / m_sampleSpacing;
				int sampleY = y / m_sampleSpacing;
				int sampleGridWidth = gridWidth / m_sampleSpacing;
				// Continent map
				continent.Map[sampleX][sampleY] = iteration == 0 ? continent.initialDeviation : Diamond(continent, sampleX, sampleY, sampleGridWidth) + Deviation(continent.diamondDeviation);
				// Biome map
				m_biome.Map[sampleX][sampleY] = Diamond(m_biome, sampleX, sampleY, sampleGridWidth) + Deviation(m_biome.diamondDeviation);
				// Terrain map
				float continentSample = (continent.Map[sampleX][sampleY] + (CM_offset * continent.initialDeviation)) * m_sampleSpacing;
				float biomeSample = m_biome.Map[sampleX][sampleY];
				continentSample *= continentSample > 0 ? landAmplitude : oceanAmplitude; // scale the point for land or ocean
				float biomeDeviation = BiomeDeviation(biomeSample, continentSample);
				if (gridWidth >= m_sampleSpacing) {
					m_terrain.Map[x][y] = continentSample;
					m_terrain.Map[x][y] += biomeDeviation;
					if (iteration > 2) {
					m_terrain.Map[x][y] += (m_terrain.diamondDeviation * Deviation(biomeDeviation)*biomeDeviationConst);
					}
				} else {
					m_terrain.Map[x][y] = Diamond(m_terrain, x, y, gridWidth);
					m_terrain.Map[x][y] += Deviation(m_terrain.diamondDeviation);
					m_terrain.Map[x][y] += (m_terrain.diamondDeviation * Deviation(biomeDeviation)*biomeDeviationConst);
				}
				//-----------
				// Square
				//-----------
				for (float rad = (x == m_terrain.width - gridWidth || y == m_terrain.width - gridWidth ? 0.f : 2.f); rad < 4.f; rad++) {
					int pointX = round(x + cos(rad*XM_PI / 2) * gridWidth);
					int pointY = round(y + sin(rad*XM_PI / 2) * gridWidth);
					sampleX = pointX / m_sampleSpacing;
					sampleY = pointY / m_sampleSpacing;
					// Continent map
					if (sampleX == 0 || sampleX == continent.width || sampleY == 0 || sampleY == continent.width) {
						// point is on the edge of the mountain
						continent.Map[sampleX][sampleY] = -continent.initialDeviation;
					}
					else {
						// use the square method to calculate the elevation
						continent.Map[sampleX][sampleY] = Square(continent, sampleX, sampleY, sampleGridWidth);
						continent.Map[sampleX][sampleY] += Deviation(continent.squareDeviation);
					}
					// Biome map
					m_biome.Map[sampleX][sampleY] = Square(m_biome, sampleX, sampleY, sampleGridWidth);
					m_biome.Map[sampleX][sampleY] += Deviation(m_biome.squareDeviation);
					// Terrain map
					float continentSquareSample = (continent.Map[sampleX][sampleY] + (CM_offset * continent.initialDeviation)) * m_sampleSpacing;
					float biomeSquareSample = m_biome.Map[sampleX][sampleY];
					continentSquareSample *= continentSquareSample > 0 ? landAmplitude : oceanAmplitude; // scale the point for land or ocean
					float biomeDeviation = BiomeDeviation(biomeSquareSample, continentSquareSample);
					if (gridWidth >= m_sampleSpacing) {
						m_terrain.Map[pointX][pointY] = continentSquareSample;
						m_terrain.Map[pointX][pointY] += biomeDeviation;
						if (iteration > 2) {
							m_terrain.Map[pointX][pointY] += (m_terrain.squareDeviation *Deviation(biomeDeviation)*biomeDeviationConst);
						}
					} else {
						m_terrain.Map[pointX][pointY] = Square(m_terrain, pointX, pointY, gridWidth);
						m_terrain.Map[pointX][pointY] += Deviation(m_terrain.squareDeviation);
						m_terrain.Map[pointX][pointY] += (m_terrain.squareDeviation *Deviation(biomeDeviation)*biomeDeviationConst);
					}
				}
			}
		}
		// decrease the random deviation range
		if (iteration >= continent.zoom) {
			continent.diamondDeviation *= (0.5f * continent.deviationDecrease);
		}
		if (iteration >= m_biome.zoom) {
			m_biome.diamondDeviation *= (0.5f * m_biome.deviationDecrease);
		}
		m_terrain.diamondDeviation *= (0.5f * m_terrain.deviationDecrease);
	}
	// Erosion filter
	InitializeErosionMap();
	
	SaveTerrain();
	CreateTerrainEntities();
}

void TerrainSystem::Update(double & elapsed)
{
	Vector3 velocity = EM->GetComponent<Components::Movement>(EM->Player(), "Movement")->Velocity;
	Erosion();
	if (velocity.Length() < 10) {
		UpdateRegions(PlayerPos()->Pos);
	}
	//m_worker = std::thread(&TerrainSystem::UpdateRegions, *this,PlayerPos()->Pos);
}

string TerrainSystem::Name()
{
	return "Terrain";
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
		return InternalHeight(terrainStream, index);
	}
	return 0.0;
}

int TerrainSystem::Width()
{
	return m_width;
}

DirectX::SimpleMath::Rectangle TerrainSystem::Area()
{
	return DirectX::SimpleMath::Rectangle(0,0,Width(),Width());
}

void TerrainSystem::SetVertex(const int & x, const int & z, const float value)
{
	/*vector<shared_ptr<Entity>> regions = EM->FindEntities(EM->ComponentMask("Terrain"));
	for (auto & region : regions) {
		shared_ptr<Components::Position> pos = EM->GetComponent<Components::Position>(region, "Position");

	}
	shared_ptr<Components::VBO> terrainVBO = EM->GetComponent<Components::VBO>(region, "VBO");

	if (terrainVBO->Vertices.size() != 0) terrainVBO->Vertices[Utility::posToIndex(z, x, m_width + 1)].position.y = m_erosionMap.Map[x][z].Height;*/
}

shared_ptr<Components::Position> TerrainSystem::PlayerPos()
{
	return EM->GetComponent<Components::Position>(EM->Player(),"Position");
}

void TerrainSystem::SaveTerrain()
{
	//-------------------------------
	// save terrain to binary file
	//-------------------------------

	// Terrain Map
	unsigned int vertexCount = (m_terrain.width + 1) * (m_terrain.width + 1);
	unique_ptr<unsigned char[]> terrainBuffer(new unsigned char[vertexCount * sizeof(short)]);

	// Normal Map
	unique_ptr<char[]> normalBuffer(new char[vertexCount * 3]); // X (8 bits) Y (8 bits) Z (8 bits)

																// Biome Map
	unsigned int sampleSpacing = m_terrain.width / m_biome.width;
	unsigned int sampleCount = (m_biome.width + 1) * (m_biome.width + 1);
	unique_ptr<unsigned char[]> biomeBuffer(new unsigned char[sampleCount * sizeof(float)]);



	unsigned int index = 0;
	unsigned int biomeIndex = 0;

	for (unsigned short vertZ = 0; vertZ <= m_terrain.width; vertZ++) {
		for (unsigned short vertX = 0; vertX <= m_terrain.width; vertX++) {
			// verticies
			float vertex = m_terrain.Map[vertX][vertZ];
			short vertexShort = (short)(vertex * 10.f);
			/*char lower = vertexShort & 0xff;
			char upper = (vertexShort >> 8) & 0xff;
			terrainBuffer.get()[index * sizeof(short)] = lower;
			terrainBuffer.get()[index * sizeof(short) + 1] = upper;*/
			std::memcpy(&terrainBuffer.get()[index * sizeof(short)], &vertexShort, sizeof(short));
			// biome
			if (vertZ % sampleSpacing == 0 && vertX % sampleSpacing == 0) {
				float floatValue = m_biome.Map[vertX / sampleSpacing][vertZ / sampleSpacing];
				std::memcpy(&biomeBuffer.get()[biomeIndex * sizeof(float)], &floatValue, sizeof(float));
				/*biomeFile.seekp(biomeIndex * sizeof(float));
				biomeFile.write((char *)charValue, sizeof(float));*/
				biomeIndex++;
			}
			// normals
			float left = signed(vertX) - 1 >= 0 ? m_terrain.Map[vertX - 1][vertZ] : vertex;
			float right = signed(vertX) + 1 <= signed(m_terrain.width) ? m_terrain.Map[vertX + 1][vertZ] : vertex;
			float up = signed(vertZ) + 1 <= signed(m_terrain.width) ? m_terrain.Map[vertX][vertZ + 1] : vertex;
			float down = signed(vertZ) - 1 >= 0 ? m_terrain.Map[vertX][vertZ - 1] : vertex;

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

	// m_terrain
	ofstream terrain(m_directory / "terrain.dat",std::ios::binary);
	terrain.write((const char *)terrainBuffer.get(), vertexCount * sizeof(short));
	terrain.close();
	// m_biome
	ofstream biome(m_directory / "biome.dat", std::ios::binary);
	biome.write((const char *)biomeBuffer.get(), sampleCount * sizeof(float));
	biome.close();
	// NormalMap
	ofstream normal(m_directory / "normal.dat", std::ios::binary);
	normal.write((const char *)normalBuffer.get(), vertexCount * 3);
	normal.close();
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
	//m_entities = EM->EntitiesContaining("Terrain");
}

void TerrainSystem::UpdateRegions(Vector3 center)
{
	
	center.y = 0;
	for (auto & entity : std::vector<shared_ptr<Entity>>(m_entities)) {
		Vector3 position = EM->GetComponent<Components::Position>(entity,"Position")->Pos;
		position.y = 0;
		double distance = Vector3::Distance(center, position);
		
		shared_ptr<Components::VBO> vbo = EM->GetComponent<Components::VBO>(entity, "VBO");
		// Update the Level Of Detail as a funtion of distance
		int lod = LOD(distance, (int)m_regionWidth);
		// Only update this VBO if the LOD has changed
		if (true || vbo->LOD != lod) {
			vbo->LOD = lod;
			
			// Get the region coordinates
			int x = (int)std::floor(position.x / (double)(int)m_regionWidth);
			int z = (int)std::floor(position.z / (double)(int)m_regionWidth);

			m_worker = std::thread([this, vbo, x, z]() {UpdateTerrainVBO(vbo, x, z);});
			m_worker.detach();
			//UpdateTerrainVBO(vbo, x, z);
			
		}
	}
}

void TerrainSystem::UpdateTerrainVBO(shared_ptr<Components::VBO> vbo, int regionX, int regionZ)
{
	//unsigned int regionIndex = Utility::posToIndex(regionX, regionZ, m_width / (int)m_regionWidth);
	//unsigned int vertexCount = ((int)m_regionWidth + 1)*((int)m_regionWidth + 1);
	//unsigned int rowSize = ((int)m_regionWidth + 1) * sizeof(short);
	//unsigned int regionSize = vertexCount * sizeof(short);
	// calculate quad size based off of LOD (Level Of Detail)
	int quadWidth = std::pow(2, vbo->LOD);
	int mapWidth = m_regionWidth / quadWidth;
	// Load the vertex array with data.
	HeightMap<float> heightMap(mapWidth,0.0,0.0,0);
	HeightMap<Vector3> normalMap(mapWidth, 0.0, 0.0, 0);
	ifstream terrainStream(m_directory / "terrain.dat", ios::binary);
	ifstream normalStream(m_directory / "normal.dat", ios::binary);
	

	//if (terrainStream.is_open() && normalStream.is_open()) {
	//	// stores the exact bytes from the file into memory
	// 		//char *terrainCharBuffer = new char[regionSize];
	//	//char *normalCharBuffer = new char[vertexCount * 3];
	//	// move start position to the region, and proceed to read each line into the Char buffers
	//	
	//	
	//	for (int vertZ = 0; vertZ <= (int)mapWidth; vertZ++) {
	//		for (int vertX = 0; vertX <= (int)mapWidth; vertX++) {
	//			int worldX = vertX * quadWidth + (int)m_regionWidth * regionX;
	//			int worldZ = vertZ * quadWidth + (int)m_regionWidth * regionZ;
	//			int index = Utility::posToIndex(worldX, worldZ, m_width + 1);
	//			// heightMap
	//			heightMap.Map[vertX][vertZ] = InternalHeight(terrainStream, index);
	//			// normalMap
	//			normalMap.Map[vertX][vertZ] = Normal(normalStream,index);
	//		}
	//	}

	//	terrainStream.close();
	//	normalStream.close();
	//	//delete[] terrainCharBuffer;
	//	//delete[] normalCharBuffer;
	//}
	// create 2 triangles (6 vertices) for every quad in the region
	vbo->Vertices.resize(6 * mapWidth * mapWidth);
	vbo->Vertices.shrink_to_fit();
	vbo->Indices.resize(6 * mapWidth * mapWidth);
	vbo->Indices.shrink_to_fit();
	int index = 0;
	for (int z = 0; z < mapWidth; z++) {
		for (int x = 0; x < mapWidth; x++) {
			if (x != 0 && z != 0) {
				float vertex = m_terrain.Map[x][z];
				// normals
				float left = signed(x) - 1 >= 0 ? m_erosionMap.Map[x - 1][z].Height + m_erosionMap.Map[x - 1][z].Water - 0.1f : vertex;
				float right = signed(x) + 1 <= signed(m_erosionMap.width) ? m_erosionMap.Map[x + 1][z].Height + m_erosionMap.Map[x + 1][z].Water - 0.1f : vertex;
				float up = signed(z - 1) + 1 <= signed(m_erosionMap.width) ? m_erosionMap.Map[x][z+1].Height + m_erosionMap.Map[x][z+1].Water - 0.1f : vertex;
				float down = signed(z - 1) - 1 >= 0 ? m_erosionMap.Map[x][z - 1].Height + m_erosionMap.Map[x][z - 1].Water - 0.1f : vertex;

				DirectX::SimpleMath::Vector3 normal = DirectX::SimpleMath::Vector3(left - right, 2.f, down - up);
				normal.Normalize();

				normalMap.Map[x][z] = normal;
			}
			// Get the indexes to the four points of the quad.

			// Upper left.
			Vector3 vertex1(
				(float)(x * quadWidth + regionX * m_regionWidth),
				m_terrain.Map[x][z],
				(float)(z * quadWidth + regionZ * m_regionWidth));
			// Upper right.
			Vector3 vertex2(
				(float)((x + 1) * quadWidth + regionX * m_regionWidth),
				m_terrain.Map[x + 1][z],
				(float)(z * quadWidth + regionZ * m_regionWidth));
			// Bottom left.
			Vector3 vertex3(
				(float)(x * quadWidth + regionX * m_regionWidth),
				m_terrain.Map[x][z + 1],
				(float)((z + 1) * quadWidth + regionZ * m_regionWidth));
			// Bottom right.
			Vector3 vertex4(
				(float)((x + 1) * quadWidth + regionX * m_regionWidth),
				m_terrain.Map[x + 1][z + 1],
				(float)((z + 1) * quadWidth + regionZ * m_regionWidth));

			/*
			1---2
			| \ |
			3---4
			*/

			// Triangle 1 - Upper left
			vbo->Vertices[index] = {
				XMFLOAT3(vertex1),										// position
				XMFLOAT3(normalMap.Map[x][z]),	// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),								// tangent
				XMFLOAT4(0.f,0.f,0.f,1.f),							// color
				XMFLOAT2(0.f,0.f)										// texture
			};
			vbo->Indices[index] = index;
			index++;
			// Triangle 1 - Bottom right.
			vbo->Vertices[index] = {
				XMFLOAT3(vertex4),												// position
				XMFLOAT3(normalMap.Map[x + 1][z + 1]),	// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),										// tangent
				XMFLOAT4(0.f,0.f,0.f,1.f),									// color
				XMFLOAT2(1.f,1.f)												// texture
			};
			vbo->Indices[index] = index;
			index++;
			// Triangle 1 - Bottom left.
			vbo->Vertices[index] = {
				XMFLOAT3(vertex3),											// position
				XMFLOAT3(normalMap.Map[x][z + 1]),	// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),									// tangent
				XMFLOAT4(0.f,0.f,0.f,1.f),								// color
				XMFLOAT2(0.f,1.f)											// texture
			};
			vbo->Indices[index] = index;
			index++;
			// Triangle 2 - Upper left.
			vbo->Vertices[index] = {
				XMFLOAT3(vertex1),										// position
				XMFLOAT3(normalMap.Map[x][z]),	// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),								// tangent
				XMFLOAT4(0.f,0.f,0.f,1.f),							// color
				XMFLOAT2(0.f,0.f)										// texture
			};
			vbo->Indices[index] = index;
			index++;
			// Triangle 2 - Upper right.
			vbo->Vertices[index] = {
				XMFLOAT3(vertex2),											// position
				XMFLOAT3(normalMap.Map[x + 1][z]),	// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),									// tangent
				XMFLOAT4(0.f,0.f,0.f,1.f),								// color
				XMFLOAT2(1.f,0.f)											// texture
			};
			vbo->Indices[index] = index;
			index++;
			// Triangle 2 - Bottom right.
			vbo->Vertices[index] = {
				XMFLOAT3(vertex4),												// position
				XMFLOAT3(normalMap.Map[x + 1][z + 1]),	// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),										// tangent
				XMFLOAT4(0.f,0.f,0.f,1.f),									// color
				XMFLOAT2(1.f,1.f)												// texture
			};
			vbo->Indices[index] = index;
			index++;
		}
	}
	normalStream.close();
	terrainStream.close();
	vbo->LODchanged = true;
}

int TerrainSystem::LOD(double distance, unsigned int modelWidth)
{
	// add integer to decrease terrain detail
	return 0 + std::min((int)std::log2(m_regionWidth),std::max(0,(int)std::floor(std::log2(distance / (double)modelWidth))));
}

void TerrainSystem::NewTerrain(DirectX::SimpleMath::Vector3 & position)
{
	shared_ptr<Entity> entity = EM->NewEntity();

	entity->AddComponent(EM->ComponentMask("Position"),std::shared_ptr<Components::Component>(
		new Components::Position(entity->ID(), position, SimpleMath::Vector3::Zero)));
	entity->AddComponent(EM->ComponentMask("Terrain"), std::shared_ptr<Components::Component>(
		new Components::Terrain(entity->ID())));
	entity->AddComponent(EM->ComponentMask("VBO"), std::shared_ptr<Components::Component>(
		new Components::VBO(entity->ID())));
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

float TerrainSystem::InternalHeight(std::ifstream & ifs, const int & index)
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
	return (float)shortVertex / 10.f;
}

float TerrainSystem::Diamond(HeightMap<float> & map, int & x, int & y, int & distance) {
	float sum = 0.f;
	sum += map.Map[x - distance][y - distance];
	sum += map.Map[x - distance][y + distance];
	sum += map.Map[x + distance][y - distance];
	sum += map.Map[x + distance][y + distance];
	return sum * 0.25f;
}
float TerrainSystem::Square(HeightMap<float> & map, int & x, int & y, int & distance) {
	float sum = 0;
	int denominator = 0;
	if (y > 0) {
		sum += map.Map[x][y - distance];
		denominator++;
	}
	if (y < map.width - 1) {
		sum += map.Map[x][y + distance];
		denominator++;
	}
	if (x > 0) {
		sum += map.Map[x - distance][y];
		denominator++;
	}
	if (x < map.width - 1) {
		sum += map.Map[x + distance][y];
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
	m_erosionMap = HeightMap<TerrainCell>(m_width, 0.f, 0.f, 0);
	// Initialize
	for (int x = 0; x <= m_width; x++) {
		for (int z = 0; z <= m_width; z++) {
			m_erosionMap.Map[x][z] = TerrainCell();
		}
	}
	for (int x = 0; x <= m_width; x++) {
		for (int z = 0; z <= m_width; z++) {
			m_terrain.Map[x][z] = std::sqrt(std::pow((float)x / 2.f, 2) + std::pow((float)z / 2.f, 2));

			TerrainCell & thisCell = m_erosionMap.Map[x][z];
			// set terrain height
			thisCell.Height = m_terrain.Map[x][z];
			//thisCell.Height = std::sqrt(std::pow((float)x / 2.f, 2) + std::pow((float)z / 2.f, 2));
			// initialize adjacent mappings
			for (int rot = 0; rot < 4; rot++) {
				int adjX = x + round(cos(rot * XM_PI / 2));
				int adjZ = z + round(sin(rot * XM_PI / 2));
				if (adjX >= 0 && adjX <= m_width && adjZ >= 0 && adjZ <= m_width) {
					thisCell.Adjacent[rot] = &m_erosionMap.Map[adjX][adjZ];
				}
				else {
					thisCell.Adjacent[rot] = nullptr;
				}
			}
		}
	}
	// Initialize the water entity
	waterEntity = EM->NewEntity();
	waterEntity->AddComponent(EM->ComponentMask("VBO"), std::shared_ptr<Components::Component>(
		new Components::VBO(waterEntity->ID())));
	shared_ptr<Components::VBO> vbo = EM->GetComponent<Components::VBO>(waterEntity, "VBO");
	vbo->Effect = "Water";
	// create 2 triangles (6 vertices) for every quad in the world
	vbo->Vertices.resize((m_width +1)* (m_width+1));
	vbo->Vertices.shrink_to_fit();
	vbo->Indices.resize(6 * m_width * m_width);
	vbo->Indices.shrink_to_fit();
}

void TerrainSystem::Erosion() {

	const int iterations = 1;
	const float timeInterval = 0.1f;
	

	static int i = 0;

	for (int z = 0; z <= m_width; z++) {
		for (int x = 0; x <= m_width; x++) {
			TerrainCell & thisCell = m_erosionMap.Map[x][z];
			if (i % 5 == 0) {
				float sourceX = x - thisCell.Velocity.x * timeInterval * 0.5;
				float sourceZ = z - thisCell.Velocity.z * timeInterval * 0.5;
				int quadX = std::floor(sourceX);
				int quadZ = std::floor(sourceZ);

				//if (quadX > 0 && quadX < m_width-1 && quadZ > 0 && quadZ < m_width-1) {
				float points[4][4];
				for (int px = -1; px < 3; px++) {
					for (int pz = -1; pz < 3; pz++) {
						if (quadX > 0 && quadX < m_width - 1 && quadZ > 0 && quadZ < m_width - 1) {
							points[px + 1][pz + 1] = m_erosionMap.Map[quadX + px][quadZ + pz].Suspended;
						}
						else {
							points[px + 1][pz + 1] = 0.f;
						}
					}
				}
				float suspended = Utility::BicubicInterpolation(points, sourceX - (float)quadX, sourceZ - (float)quadZ);

				/*float suspended = Utility::InterpolateQuad(sourceX - (float)quadX, sourceZ - (float)quadZ,
				m_erosionMap.Map[quadX][quadZ].Suspended,
				m_erosionMap.Map[quadX + 1][quadZ].Suspended,
				m_erosionMap.Map[quadX + 1][quadZ + 1].Suspended,
				m_erosionMap.Map[quadX][quadZ + 1].Suspended
				);*/
				thisCell.NextSuspended += suspended;
				
				thisCell.Evaporate(timeInterval);
				if ((i / 500) % 2 == 0) {
					if (std::abs(z - m_width / 2) <= 3 && std::abs(x - m_width / 2) <= 3) thisCell.Rain(timeInterval);
				}

				thisCell.UpdateGradient();
			}
			else if (i % 5 == 1) {
				thisCell.UpdateFlux(timeInterval);
			}
			else if (i % 5 == 2) {
				thisCell.UpdateWater(timeInterval);
			}
			else if (i % 5 == 3) {
				thisCell.Erode(timeInterval);
				//thisCell.Deposit(timeInterval);

			}
			else if (i % 5 == 4) {
				thisCell.ApplyDeltas();
				
					//thisCell.NextSuspended = 0.f;
				//}
			}
		}
	}
	
	// Update the VBO
	shared_ptr<Components::VBO> vbo = EM->GetComponent<Components::VBO>(waterEntity,"VBO");

	int indexBufferIndex = 0;
	int vertexBufferIndex = 0;
	for (int z = 0; z <= m_width; z++) {
		for (int x = 0; x <= m_width; x++) {
			// update Terrain
			m_terrain.Map[x][z] = m_erosionMap.Map[x][z].Height;
			
			Vector3 vertex(
				(float)x,
				m_erosionMap.Map[x][z].Height + m_erosionMap.Map[x][z].Water- 0.1f,
				(float)z);
			
			vbo->Vertices[vertexBufferIndex] = {
				XMFLOAT3(vertex),										// position
				XMFLOAT3(0.f,1.f,0.f),									// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),								// tangent
				XMFLOAT4(0.f,0.f,255.f,0.f),							// color
				XMFLOAT2(0.f,0.f)										// texture
			};

			if (x != 0 && z != 0) {
				float vertex = m_erosionMap.Map[x][z - 1].Height + m_erosionMap.Map[x][z - 1].Water - 0.1f;
				// normals
				float left = signed(x) - 1 >= 0 ? m_erosionMap.Map[x - 1][z - 1].Height + m_erosionMap.Map[x - 1][z - 1].Water - 0.1f : vertex;
				float right = signed(x) + 1 <= signed(m_erosionMap.width) ? m_erosionMap.Map[x + 1][z-1].Height  + m_erosionMap.Map[x + 1][z - 1].Water - 0.1f : vertex;
				float up = signed(z-1) + 1 <= signed(m_erosionMap.width) ? m_erosionMap.Map[x][z].Height + m_erosionMap.Map[x][z].Water - 0.1f : vertex;
				float down = signed(z-1) - 1 >= 0 ? m_erosionMap.Map[x][z - 2].Height + m_erosionMap.Map[x][z - 2].Water - 0.1f : vertex;

				DirectX::SimpleMath::Vector3 normal = DirectX::SimpleMath::Vector3(left - right, 2.f, down - up);
				normal.Normalize();
				vbo->Vertices[vertexBufferIndex - (m_erosionMap.width + 1)].normal = normal;
			}
			vertexBufferIndex++;
			if (x != m_width && z != m_width) {
				/*
				1---2
				| \ |
				3---4
				*/
				int i1 = Utility::posToIndex(x, z, m_width + 1);
				int i2 = Utility::posToIndex(x+1, z, m_width + 1);
				int i3 = Utility::posToIndex(x, z+1, m_width + 1);
				int i4 = Utility::posToIndex(x+1, z + 1, m_width + 1);

				// Left triangle
				vbo->Indices[indexBufferIndex] = i1;
				indexBufferIndex++;
				vbo->Indices[indexBufferIndex] = i4;
				indexBufferIndex++;
				vbo->Indices[indexBufferIndex] = i3;
				indexBufferIndex++;
				// Right
				vbo->Indices[indexBufferIndex] = i1;
				indexBufferIndex++;
				vbo->Indices[indexBufferIndex] = i2;
				indexBufferIndex++;
				vbo->Indices[indexBufferIndex] = i4;
				indexBufferIndex++;
			}
		}
	}
	vbo->LODchanged = true;
	// Update VBOS
	i++;
}
//void TerrainSystem::Erosion() {
//	HeightMap<TerrainCell> erosionMap(m_width,0,0,0);
//	HeightMap<TerrainCell> deltaMap(m_width, 0, 0, 0);
//	const float rainHeight = 8.f;
//	const float rainQty = 10.f;
//	const float g = 9.8;
//	const float maxDissolved = 0.3;
//	const float inertiaLoss = 0.1;
//	const int iterations = 10;
//	// begin the simulations
//	for (int i = 0; i < iterations; i++) {
//		for (int x = 0; x <= m_width; x++) {
//			for (int z = 0; z <= m_width; z++) {
//				if (m_terrain.Map[x][z] >= -32) {
//					TerrainCell & thisCell = erosionMap.Map[x][z];
//					thisCell.Height = m_terrain.Map[x][z];
//
//					// add rain
//					if (i == 0 && thisCell.Height > rainHeight) {
//						deltaMap.Map[x][z].Water += rainQty;
//					}
//
//					// iterate over adjacent cells
//					float volume = thisCell.Volume();
//
//					vector<TerrainCell*> spreadable;
//
//					float height_sum = thisCell.Height + volume;
//					
//					// find the spreadable cells and equalization level
//					for (int rot = 0; rot < 4; rot++) {
//						int adjX = x + round(cos(rot * XM_PI / 2));
//						int adjZ = z + round(sin(rot * XM_PI / 2));
//						if (adjX >= 0 && adjX <= m_terrain.width && adjZ >= 0 && adjZ <= m_terrain.width) {
//							erosionMap.Map[adjX][adjZ].Height = m_terrain.Map[adjX][adjZ];
//							erosionMap.Map[adjX][adjZ].X = adjX;
//							erosionMap.Map[adjX][adjZ].Z = adjZ;
//
//							float adjVolume = erosionMap.Map[adjX][adjZ].Volume();
//							if (erosionMap.Map[adjX][adjZ].Height + adjVolume < thisCell.Height + volume) {
//								spreadable.push_back(&erosionMap.Map[adjX][adjZ]);
//								height_sum += erosionMap.Map[adjX][adjZ].Height + adjVolume;
//							}
//						}
//					}
//					float equalization = height_sum / (spreadable.size() + 1);
//					vector<float> dh;
//					float dh_sum = 0.f;
//					float transferVolume = std::min(thisCell.Volume(), thisCell.Height + volume - equalization);
//					// find delta heights
//					for (TerrainCell * cell : spreadable) {
//						float this_dh = std::min(transferVolume, (thisCell.Height + volume) - (cell->Height + cell->Volume()));
//						dh_sum += this_dh;
//						dh.push_back(this_dh);
//					}
//
//					// apply the transfer
//					int c = 0;
//					if (dh_sum > 0.f) {
//						for (TerrainCell * cell : spreadable) {
//							float directionalBias = 1.0;
//							if (cell->X - thisCell.X > 0) {
//								directionalBias +=
//							}
//							else if (cell->X - thisCell.X < 0) {
//								directionalBias -= Utility::Inverse(cell->Velocity.x, 1.0, 0.0, 1.0);
//							}
//							if (cell->Z - thisCell.Z > 0) {
//								directionalBias += Utility::Inverse(cell->Velocity.z, 1.0, 0.0, 1.0);
//							}
//							else if (cell->Z - thisCell.Z < 0) {
//								directionalBias -= Utility::Inverse(cell->Velocity.z, 1.0, 0.0, 1.0);
//							}
//
//							float percent_transfer = (dh[c] / dh_sum) * (transferVolume / volume);
//							float soilTransferred = thisCell.Dissolved * percent_transfer;
//							float waterTransferred = thisCell.Water * percent_transfer;
//							float volumeTransferred = (soilTransferred + waterTransferred);
//							deltaMap.Map[cell->X][cell->Z].Dissolved += soilTransferred;
//							deltaMap.Map[cell->X][cell->Z].Water += waterTransferred;
//							deltaMap.Map[x][z].Dissolved -= soilTransferred;
//							deltaMap.Map[x][z].Water -= waterTransferred;
//
//							// calculate the Inertia that was transferred
//							float drop = thisCell.Height + volume - (cell->Height + cell->Volume() + volumeTransferred);
//							Vector3 inertia(cell->X - x, drop, cell->Z - z);
//							inertia.Normalize();
//							inertia *= std::sqrt(2 * g * drop) * volumeTransferred;
//
//							Vector3 currentInertia = cell->Velocity * (cell->Volume());
//
//							deltaMap.Map[cell->X][cell->Z].Velocity += ((currentInertia + inertia) / (cell->Volume() + volumeTransferred));
//							deltaMap.Map[cell->X][cell->Z].Velocity -= cell->Velocity;
//							deltaMap.Map[cell->X][cell->Z].Velocity *= (1 - inertiaLoss);
//
//							c++;
//						}
//					}
//				}
//			}
//		}
//		// Apply deltas
//		for (int x = 0; x <= m_width; x++) {
//			for (int z = 0; z <= m_width; z++) {
//				if (m_terrain.Map[x][z] > 0) {
//					TerrainCell & thisCell = erosionMap.Map[x][z];
//					thisCell.ApplyDeltas(deltaMap.Map[x][z]);
//
//					//thisCell.Erode();
//					//thisCell.Deposit();
//					m_terrain.Map[x][z] = thisCell.Height;
//
//					// dump all dissolved soil at the end
//					if (i == iterations - 1) {
//						//m_terrain.Map[x][z] += thisCell.Dissolved;
//						m_terrain.Map[x][z] += thisCell.Water;
//					}
//
//				}
//			}
//		}
//	}
//}

//void TerrainSystem::Erosion() {
//	HeightMap<TerrainCell> erosionMap(m_width, 0, 0, 0);
//	HeightMap<TerrainCell> deltaMap(m_width, 0, 0, 0);
//	const float rainHeight = 8.f;
//	const float rainQty = 10.f;
//	const float g = 9.8;
//	const float maxDissolved = 0.3;
//	const float inertiaLoss = 0.1;
//	const int iterations = 2;
//	// begin the simulations
//	for (int i = 0; i < iterations; i++) {
//		for (int x = 0; x <= m_width; x++) {
//			for (int z = 0; z <= m_width; z++) {
//				if (m_terrain.Map[x][z] >= -32) {
//					TerrainCell & thisCell = erosionMap.Map[x][z];
//					thisCell.Height = m_terrain.Map[x][z];
//					thisCell.X = x;
//					thisCell.Z = z;
//
//					
//					// add rain
//					if (i == 0 && thisCell.Height > rainHeight) {
//						deltaMap.Map[x][z].Water += rainQty;
//					}
//
//					// iterate over adjacent cells
//					float volume = thisCell.Volume();
//
//					vector<TerrainCell> cells;
//					cells.push_back(thisCell);
//					// find the spreadable cells and equalization level
//					for (int rot = 0; rot < 4; rot++) {
//						int adjX = x + round(cos(rot * XM_PI / 2));
//						int adjZ = z + round(sin(rot * XM_PI / 2));
//						if (adjX >= 0 && adjX <= m_terrain.width && adjZ >= 0 && adjZ <= m_terrain.width) {
//							TerrainCell* adjacent = &erosionMap.Map[adjX][adjZ];
//							adjacent->Height = m_terrain.Map[adjX][adjZ];
//							adjacent->X = adjX;
//							adjacent->Z = adjZ;
//
//							volume += adjacent->Volume();
//							cells.push_back(*adjacent);
//						}
//					}
//					// sort the deltaHeights ascending
//					std::sort(cells.begin(), cells.end(),TerrainCell::Higher);
//					// find the equalization heights for each tier
//					vector<float> eqHeights;
//					
//					for (int tier = 0; tier < 5 && volume > 0.f; tier++) {
//						float eqHeight = 0.0;
//						if (tier >= cells.size() - 1) {
//							eqHeight = volume / tier;
//						}
//						else {
//							eqHeight = std::min(volume / tier, cells[tier + 1].Height - cells[tier].Height);
//						}
//						volume -= eqHeight * tier;
//						
//						eqHeights.push_back(eqHeight);
//
//						
//					}
//					
//					for (int j = 0; j < cells.size(); j++) {
//						if (cells[j].X == thisCell.X && cells[j].Z == thisCell.Z) {
//							float totalHeight = 0.0;
//							for (int prev = 0; prev < eqHeights.size() && prev <= j; prev++) {
//								totalHeight += eqHeights[prev];
//							}
//							if (totalHeight > 0.f) {
//								int test = 0;
//							}
//							// set delta height for this cell
//							deltaMap.Map[cells[j].X][cells[j].Z].Water += totalHeight - cells[j].Volume();
//						}
//					}
//
//					// set deltas to achieve equalization
//
//
//					vector<float> dh;
//					float dh_sum = 0.f;
//					//float transferVolume = std::min(thisCell.Volume(), thisCell.Height + volume - equalization);
//					//// find delta heights
//					//for (TerrainCell & cell : cells) {
//					//	float this_dh = std::min(transferVolume, (thisCell.Height + volume) - (cell.Height + cell.Volume()));
//					//	dh_sum += this_dh;
//					//	dh.push_back(this_dh);
//					//}
//
//					//// apply the transfer
//					//int c = 0;
//					//if (dh_sum > 0.f) {
//					//for (TerrainCell & cell : cells) {
//					//	//float percent_transfer = (dh[c] / dh_sum) * (transferVolume / volume);
//					//	//float soilTransferred = thisCell.Dissolved * percent_transfer;
//					//	float waterTransferred = thisCell;
//					//	//float volumeTransferred = (soilTransferred + waterTransferred);
//					//	deltaMap.Map[cell.X][cell.Z].Dissolved += soilTransferred;
//					//	deltaMap.Map[cell.X][cell.Z].Water += waterTransferred;
//					//	deltaMap.Map[x][z].Dissolved -= soilTransferred;
//					//	deltaMap.Map[x][z].Water -= waterTransferred;
//
//					//	// calculate the Inertia that was transferred
//					//	float drop = thisCell.Height + volume - (cell.Height + cell.Volume() + volumeTransferred);
//					//	Vector3 inertia(cell.X - x, drop, cell.Z - z);
//					//	inertia.Normalize();
//					//	inertia *= std::sqrt(2 * g * drop) * volumeTransferred;
//
//					//	Vector3 currentInertia = cell.Velocity * (cell.Volume());
//
//					//	deltaMap.Map[cell.X][cell.Z].Velocity += ((currentInertia + inertia) / (cell.Volume() + volumeTransferred));
//					//	deltaMap.Map[cell.X][cell.Z].Velocity -= cell.Velocity;
//					//	deltaMap.Map[cell.X][cell.Z].Velocity *= (1 - inertiaLoss);
//
//					//	c++;
//					//}
//					
//				}
//			}
//		}
//		// Apply deltas
//		for (int x = 0; x <= m_width; x++) {
//			for (int z = 0; z <= m_width; z++) {
//				if (m_terrain.Map[x][z] > 0) {
//					TerrainCell & thisCell = erosionMap.Map[x][z];
//					thisCell.ApplyDeltas(deltaMap.Map[x][z]);
//					
//
//					//thisCell.Erode();
//					//thisCell.Deposit();
//					m_terrain.Map[x][z] = thisCell.Height;
//
//					// dump all dissolved soil at the end
//					if (i == iterations - 1) {
//						//m_terrain.Map[x][z] += thisCell.Dissolved;
//						m_terrain.Map[x][z] += thisCell.Water;
//					}
//
//				}
//			}
//		}
//	}
//}

//const float depositK = 0.25f;
//const float erodeK = 1.f;
//const float frictionK = 0.05;
//struct Water {
//	Vector3 KE;
//	Vector3 deltaKE;
//	float water;
//	float deltaW;
//	float soil;
//	float deltaS;
//	float lowestNeighbor;
//
//	Water() {
//		KE = Vector3(0.f, 0.f, 0.f);
//		deltaKE = Vector3(0.f, 0.f, 0.f);
//		water = 1.f;
//		deltaW = 0.f;
//		soil = 0.f;
//		deltaS = 0.f;
//	}
//};
//// create the water map
//vector< vector<Water> > waterMap = vector< vector<Water> >(m_terrain.width + 1, vector<Water>(m_terrain.width + 1));
//// update
//for (int iteration = 0; iteration < 5; iteration++) {
//	for (int x = 0; x <= m_terrain.width; x++) {
//		for (int y = 0; y <= m_terrain.width; y++) {
//			float volume = waterMap[x][y].water + waterMap[x][y].soil;
//			float deltaW = 0.f;
//			float deltaS = 0.f;
//			// add valid ajacent cells to calculation
//			waterMap[x][y].lowestNeighbor = m_terrain.Map[x][y];
//			for (int rot = 0; rot < 4; rot++) {
//				int adjX = x + round(cos(rot * XM_PI / 2));
//				int adjY = y + round(sin(rot * XM_PI / 2));
//				if (adjX >= 0 && adjX <= m_terrain.width && adjY >= 0 && adjY <= m_terrain.width) {
//					float adjVolume = waterMap[adjX][adjY].water + waterMap[adjX][adjY].soil;
//					if (m_terrain.Map[adjX][adjY] < waterMap[x][y].lowestNeighbor) {
//						waterMap[x][y].lowestNeighbor = m_terrain.Map[adjX][adjY];
//					}
//					float difference = (m_terrain.Map[adjX][adjY] + adjVolume) - (m_terrain.Map[x][y] + volume);
//					float transferVolume = std::max(-volume / 4, std::min(difference / 4, adjVolume / 4));
//					// material transfer
//					float overlap;
//					if (transferVolume > 0) {
//						deltaW += (waterMap[adjX][adjY].water / adjVolume) * transferVolume;
//						deltaS += (waterMap[adjX][adjY].soil / adjVolume) * transferVolume;
//						overlap = m_terrain.Map[x][y] + volume - m_terrain.Map[adjX][adjY];
//					}
//					else {
//						deltaW += (waterMap[x][y].water / volume) * transferVolume;
//						deltaS += (waterMap[x][y].soil / volume) * transferVolume;
//						overlap = m_terrain.Map[adjX][adjY] + adjVolume - m_terrain.Map[x][y];
//					}
//					// soil diffusion
//					if (overlap > 0) {
//						deltaS += (((waterMap[adjX][adjY].soil / adjVolume) * overlap) + ((waterMap[x][y].soil / volume) * overlap)) / 2 - ((waterMap[x][y].soil / volume) * overlap);
//					}
//					// add gravitational and kinetic energy
//					if (abs(transferVolume) > 0) {
//						if (adjY == y) {
//							waterMap[x][y].deltaKE.x += (transferVolume * 9.8 * difference) + (transferVolume > 0 ? waterMap[adjX][adjY].KE.x * (transferVolume / (waterMap[adjX][adjY].water + waterMap[adjX][adjY].soil)) : waterMap[x][y].KE.x * (transferVolume / (waterMap[x][y].water + waterMap[x][y].soil)));
//						}
//						if (adjX == x) {
//							waterMap[x][y].deltaKE.z += (transferVolume * 9.8 * difference) + (transferVolume > 0 ? waterMap[adjX][adjY].KE.z * (transferVolume / (waterMap[adjX][adjY].water + waterMap[adjX][adjY].soil)) : waterMap[x][y].KE.z * (transferVolume / (waterMap[x][y].water + waterMap[x][y].soil)));
//						}
//					}
//				}
//			}
//			// set final delta values
//			waterMap[x][y].deltaW += deltaW;
//			waterMap[x][y].deltaS += deltaS;
//		}
//	}
//	for (int x = 0; x <= m_terrain.width; x++) {
//		for (int y = 0; y <= m_terrain.width; y++) {
//			// evaporation
//			waterMap[x][y].water *= 0.75;
//			// add surface water
//			waterMap[x][y].water += 1;
//			// update cell
//			waterMap[x][y].KE += waterMap[x][y].deltaKE;
//			waterMap[x][y].deltaKE = Vector3(0.f, 0.f, 0.f);
//			waterMap[x][y].water += waterMap[x][y].deltaW;
//			waterMap[x][y].deltaW = 0;
//			waterMap[x][y].soil += waterMap[x][y].deltaS;
//			waterMap[x][y].deltaS = 0;
//			// loss of energy
//			waterMap[x][y].KE *= frictionK;
//			// update terrain
//			if (waterMap[x][y].water > 0) {
//				float volume = waterMap[x][y].water + waterMap[x][y].soil;
//				float soil = 0.f;
//				Vector3 velocity(sqrt((2 * waterMap[x][y].KE.x) / volume), 0.f, sqrt((2 * waterMap[x][y].KE.z) / volume));
//				float speed = velocity.Length();
//
//				// erode
//				soil = Sigmoid(speed, (waterMap[x][y].lowestNeighbor - m_terrain.Map[x][y]) * erodeK, 0.f, 5.f);
//				m_terrain.Map[x][y] += soil;
//				waterMap[x][y].soil -= soil;
//				// deposit
//				//if (velocity < 1.f && waterMap[x][y].soil > 3) {
//				soil = (waterMap[x][y].soil * depositK) - Sigmoid(speed, waterMap[x][y].soil * depositK, 0.f, 5.f);
//				m_terrain.Map[x][y] += soil;
//				waterMap[x][y].soil -= soil;
//				//}
//			}
//		}
//	}
//}