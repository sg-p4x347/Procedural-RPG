#include "pch.h"
#include "TerrainSystem.h"
#include "Terrain.h"
#include "Position.h"
#include "VBO.h"

TerrainSystem::TerrainSystem(
	shared_ptr<EntityManager> & entityManager, 
	vector<string> & components, 
	unsigned short updatePeriod, 
	unsigned int regionWidth,
	Filesystem::path directory
)
	: System(entityManager,components,updatePeriod), 
	m_regionWidth(regionWidth)
{
	m_directory = directory.append(Name());
	Filesystem::create_directory(m_directory);
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
	JsonParser terrainMap = config["m_terrain"];
	m_width = terrainMap["width"].To<int>();
	m_terrain = HeightMap(m_width, m_width * terrainMap["initialDeviation"].To<double>(), terrainMap["deviationDecrease"].To<double>(), 0);
	// Continent map
	JsonParser continentMap = config["continentMap"];
	const float CM_offset = continentMap["offset"].To<double>();
	HeightMap continent = HeightMap(m_terrain.width / m_sampleSpacing, (m_terrain.width / m_sampleSpacing) * continentMap["initialDeviation"].To<double>(), continentMap["deviationDecrease"].To<double>(), continentMap["zoom"].To<int>());
	// Biome map
	JsonParser biomeMap = config["m_biome"];
	m_biome = HeightMap(m_terrain.width / m_sampleSpacing, biomeMap["initialDeviation"].To<double>(), biomeMap["deviationDecrease"].To<double>(), biomeMap["zoom"].To<int>());

	//Load(); TEMP
	// initialize the corners
	for (int x = 0; x <= continent.width; x += continent.width) {
		for (int y = 0; y <= continent.width; y += continent.width) {
			continent.map[x][y] = -continent.initialDeviation;
			m_biome.map[x][y] = Deviation(m_biome.initialDeviation);
			float continentZ = (continent.map[x][y] + (CM_offset * continent.initialDeviation)) * m_sampleSpacing;
			m_terrain.map[x*m_sampleSpacing][y*m_sampleSpacing] = continentZ * oceanAmplitude + BiomeDeviation(m_biome.map[x][y], continentZ);
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
				continent.map[sampleX][sampleY] = iteration == 0 ? continent.initialDeviation : Diamond(continent, sampleX, sampleY, sampleGridWidth) + Deviation(continent.diamondDeviation);
				// Biome map
				m_biome.map[sampleX][sampleY] = Diamond(m_biome, sampleX, sampleY, sampleGridWidth) + Deviation(m_biome.diamondDeviation);
				// Terrain map
				float continentSample = (continent.map[sampleX][sampleY] + (CM_offset * continent.initialDeviation)) * m_sampleSpacing;
				float biomeSample = m_biome.map[sampleX][sampleY];
				continentSample *= continentSample > 0 ? landAmplitude : oceanAmplitude; // scale the point for land or ocean
				float biomeDeviation = BiomeDeviation(biomeSample, continentSample);
				if (gridWidth >= m_sampleSpacing) {
					m_terrain.map[x][y] = continentSample;
					m_terrain.map[x][y] += biomeDeviation;
					if (iteration > 2) {
					m_terrain.map[x][y] += (m_terrain.diamondDeviation * Deviation(biomeDeviation)*biomeDeviationConst);
					}
				} else {
					m_terrain.map[x][y] = Diamond(m_terrain, x, y, gridWidth);
					m_terrain.map[x][y] += Deviation(m_terrain.diamondDeviation);
					m_terrain.map[x][y] += (m_terrain.diamondDeviation * Deviation(biomeDeviation)*biomeDeviationConst);
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
						continent.map[sampleX][sampleY] = -continent.initialDeviation;
					}
					else {
						// use the square method to calculate the elevation
						continent.map[sampleX][sampleY] = Square(continent, sampleX, sampleY, sampleGridWidth);
						continent.map[sampleX][sampleY] += Deviation(continent.squareDeviation);
					}
					// Biome map
					m_biome.map[sampleX][sampleY] = Square(m_biome, sampleX, sampleY, sampleGridWidth);
					m_biome.map[sampleX][sampleY] += Deviation(m_biome.squareDeviation);
					// Terrain map
					float continentSquareSample = (continent.map[sampleX][sampleY] + (CM_offset * continent.initialDeviation)) * m_sampleSpacing;
					float biomeSquareSample = m_biome.map[sampleX][sampleY];
					continentSquareSample *= continentSquareSample > 0 ? landAmplitude : oceanAmplitude; // scale the point for land or ocean
					float biomeDeviation = BiomeDeviation(biomeSquareSample, continentSquareSample);
					if (gridWidth >= m_sampleSpacing) {
						m_terrain.map[pointX][pointY] = continentSquareSample;
						m_terrain.map[pointX][pointY] += biomeDeviation;
						if (iteration > 2) {
							m_terrain.map[pointX][pointY] += (m_terrain.squareDeviation *Deviation(biomeDeviation)*biomeDeviationConst);
						}
					} else {
						m_terrain.map[pointX][pointY] = Square(m_terrain, pointX, pointY, gridWidth);
						m_terrain.map[pointX][pointY] += Deviation(m_terrain.squareDeviation);
						m_terrain.map[pointX][pointY] += (m_terrain.squareDeviation *Deviation(biomeDeviation)*biomeDeviationConst);
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
	//Erosion();
}

void TerrainSystem::SyncEntities()
{
	if (!m_player) {
		m_player = m_entityManager->FindEntity(
			m_entityManager->ComponentMask(vector<string>{ "Player", "Position" })
		);
	}
	
}

void TerrainSystem::Update()
{
	UpdateRegions(m_player->Pos);
}

string TerrainSystem::Name()
{
	return "Terrain";
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
			float vertex = m_terrain.map[vertX][vertZ];
			terrainBuffer.get()[index * sizeof(short)] = short(vertex * 10) & 0xff;
			terrainBuffer.get()[index * sizeof(short) + 1] = (short(vertex * 10) >> 8) & 0xff;
			// biome
			if (vertZ % sampleSpacing == 0 && vertX % sampleSpacing == 0) {
				float floatValue = m_biome.map[vertX / sampleSpacing][vertZ / sampleSpacing];
				memcpy(&biomeBuffer.get()[biomeIndex * sizeof(float)], &floatValue, sizeof(float));
				/*biomeFile.seekp(biomeIndex * sizeof(float));
				biomeFile.write((char *)charValue, sizeof(float));*/
				biomeIndex++;
			}
			// normals
			float left = signed(vertX) - 1 >= 0 ? m_terrain.map[vertX - 1][vertZ] : vertex;
			float right = signed(vertX) + 1 <= signed(m_terrain.width) ? m_terrain.map[vertX + 1][vertZ] : vertex;
			float up = signed(vertZ) + 1 <= signed(m_terrain.width) ? m_terrain.map[vertX][vertZ + 1] : vertex;
			float down = signed(vertZ) - 1 >= 0 ? m_terrain.map[vertX][vertZ - 1] : vertex;

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
	ofstream terrain(m_directory.append("terrain.dat"));
	terrain.write((const char *)terrainBuffer.get(), vertexCount * sizeof(short));
	terrain.close();
	// m_biome
	ofstream biome(m_directory.append("biome.dat"));
	biome.write((const char *)biomeBuffer.get(), sampleCount * sizeof(float));
	biome.close();
	// NormalMap
	ofstream normal(m_directory.append("normal.dat"));
	normal.write((const char *)normalBuffer.get(), vertexCount * 3);
	normal.close();
}

void TerrainSystem::CreateTerrainEntities()
{
	for (int x = 0; x < m_width / m_regionWidth; x++) {
		for (int z = 0; z < m_width / m_regionWidth; z++) {
			Vector3 position(
				((double)x + (double)m_regionWidth*0.5) * (double)m_regionWidth,
				0.0,
				((double)z + (double)m_regionWidth*0.5) * (double)m_regionWidth
			);
			NewTerrain(position);
		}
	}
	m_entities = m_entityManager->EntitiesContaining(
		"Terrain",
		m_entityManager->ComponentMask(vector<string>{"Position", "VBO"})
	);
}

void TerrainSystem::UpdateRegions(Vector3 center)
{
	center.y = 0;
	for (shared_ptr<Entity> & entity : m_entities) {
		Vector3 position = entity->GetComponent<Component::Position>("Position")->Pos;
		position.y = 0;
		double distance = Vector3::Distance(center, position);
		int lod = LOD(distance, m_regionWidth);
		shared_ptr<Component::VBO> vbo = entity->GetComponent<Component::VBO>("VBO");
		int x = (int)std::floor(position.x / (double)m_regionWidth);
		int z = (int)std::floor(position.z / (double)m_regionWidth);
		UpdateTerrainVBO(vbo, x, z, lod);
	}
}

void TerrainSystem::UpdateTerrainVBO(shared_ptr<Component::VBO> vbo, int x, int z, int lod)
{
	unsigned int regionIndex = posToIndex(x, z, m_width / m_regionWidth);
	unsigned int vertexCount = (m_regionWidth + 1)*(m_regionWidth + 1);
	unsigned int rowSize = (m_regionWidth + 1) * sizeof(short);
	unsigned int regionSize = vertexCount * sizeof(short);

	// Load the vertex array with data.
	vector<float> heightMap;
	vector<Vector3> normalMap;
	ifstream terrainStream(m_directory.append( "terrain.dat"), ios::binary);
	ifstream normalStream(m_directory.append("normal.dat"), ios::binary);
	if (terrainStream.is_open() && normalStream.is_open()) {
		// stores the exact bytes from the file into memory
		char *terrainCharBuffer = new char[regionSize];
		char *normalCharBuffer = new char[vertexCount * 3];
		// move start position to the region, and proceed to read each line into the Char buffers
		for (int vertZ = 0; vertZ <= m_regionWidth; vertZ++) {
			for (int vertX = 0; vertX <= m_regionWidth; vertX++) {
				int index = posToIndex(vertX + x * m_regionWidth, vertZ + z * m_regionWidth, m_width + 1);
				// heightMap
				char shortBuffer[2];
				terrainStream.seekg(index * sizeof(short));
				terrainStream.read((char *)shortBuffer, sizeof(short));
				unsigned char lower = (unsigned char)shortBuffer[0];
				unsigned short upper = (unsigned short)shortBuffer[1] << 8;

				float vertex = short(upper | lower) / 10.f;
				heightMap.push_back(vertex);
				// normalMap
				char normalBuffer[3];
				normalStream.seekg(index * 3 * sizeof(char));
				normalStream.read((char *)normalBuffer, 3 * sizeof(char));
				Vector3 normal = Vector3(float(normalBuffer[0]), float(normalBuffer[1]), float(normalBuffer[2]));
				normal.Normalize();
				normalMap.push_back(normal);
			}
		}

		terrainStream.close();
		normalStream.close();
		delete[] terrainCharBuffer;
		delete[] normalCharBuffer;
	}
	// create 2 triangles (6 vertices) for every quad in the region
	vbo->Vertices.resize(m_regionWidth * m_regionWidth * 6);
	vbo->Indices.resize(m_regionWidth * m_regionWidth * 6);

	int index = 0;
	for (int z = 0; z < m_regionWidth; z++) {
		for (int x = 0; x < m_regionWidth; x++) {
			// Get the indexes to the four points of the quad.
			Vector3 vertex1 = Vector3(float(x + (x * m_regionWidth)), heightMap[((m_regionWidth + 1) * z) + x], float(z + (z * m_regionWidth)));          // Upper left.
			Vector3 vertex2 = Vector3(float(x + 1 + (x*m_regionWidth)), heightMap[((m_regionWidth + 1) * z) + (x + 1)], float(z + (z * m_regionWidth)));      // Upper right.
			Vector3 vertex3 = Vector3(float(x + (x*m_regionWidth)), heightMap[((m_regionWidth + 1) * (z + 1)) + x], float(z + 1 + (z * m_regionWidth)));      // Bottom left.
			Vector3 vertex4 = Vector3(float(x + 1 + (x*m_regionWidth)), heightMap[((m_regionWidth + 1) * (z + 1)) + (x + 1)], float(z + 1 + (z * m_regionWidth)));  // Bottom right.

			/*
			1---2
			| \ |
			3---4
			*/																																									


			// Triangle 1 - Upper left
			vbo->Vertices[index] = {
				XMFLOAT3(vertex1),										// position
				XMFLOAT3(normalMap[posToIndex(x,z,m_regionWidth + 1)]),	// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),								// tangent
				XMFLOAT4(1.f,255.f,1.f,1.f),							// color
				XMFLOAT2(0.f,0.f)										// texture
			};
			vbo->Indices[index] = index;
			index++;
			// Triangle 1 - Bottom right.
			vbo->Vertices[index] = {
				XMFLOAT3(vertex4),												// position
				XMFLOAT3(normalMap[posToIndex(x + 1,z + 1,m_regionWidth + 1)]),	// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),										// tangent
				XMFLOAT4(1.f,255.f,1.f,1.f),									// color
				XMFLOAT2(1.f,1.f)												// texture
			};
			vbo->Indices[index] = index;
			index++;
			// Triangle 1 - Bottom left.
			vbo->Vertices[index] = {
				XMFLOAT3(vertex3),											// position
				XMFLOAT3(normalMap[posToIndex(x,z + 1,m_regionWidth + 1)]),	// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),									// tangent
				XMFLOAT4(1.f,255.f,1.f,1.f),								// color
				XMFLOAT2(0.f,1.f)											// texture
			};
			vbo->Indices[index] = index;
			index++;
			// Triangle 2 - Upper left.
			vbo->Vertices[index] = {
				XMFLOAT3(vertex1),										// position
				XMFLOAT3(normalMap[posToIndex(x,z,m_regionWidth + 1)]),	// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),								// tangent
				XMFLOAT4(1.f,255.f,1.f,1.f),							// color
				XMFLOAT2(0.f,0.f)										// texture
			};
			vbo->Indices[index] = index;
			index++;
			// Triangle 2 - Upper right.
			vbo->Vertices[index] = {
				XMFLOAT3(vertex2),											// position
				XMFLOAT3(normalMap[posToIndex(x + 1,z,m_regionWidth + 1)]),	// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),									// tangent
				XMFLOAT4(1.f,255.f,1.f,1.f),								// color
				XMFLOAT2(1.f,0.f)											// texture
			};
			vbo->Indices[index] = index;
			index++;
			// Triangle 2 - Bottom right.
			vbo->Vertices[index] = {
				XMFLOAT3(vertex4),												// position
				XMFLOAT3(normalMap[posToIndex(x + 1,z + 1,m_regionWidth + 1)]),	// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),										// tangent
				XMFLOAT4(1.f,255.f,1.f,1.f),									// color
				XMFLOAT2(1.f,1.f)												// texture
			};
			vbo->Indices[index] = index;
			index++;
		}
	}
}

int TerrainSystem::LOD(double distance, unsigned int modelWidth)
{
	return (int)std::floor(std::log2(distance / (double)modelWidth));
}

void TerrainSystem::NewTerrain(DirectX::SimpleMath::Vector3 & position)
{
	unsigned int id = m_entityManager->NewEntity();
	m_entityManager->AttachComponent(shared_ptr<Component::Position>(new Component::Position(id,position, DirectX::SimpleMath::Vector3::Zero)));
	m_entityManager->AttachComponent(shared_ptr<Component::Terrain>(new Component::Terrain(id)));
	m_entityManager->AttachComponent(shared_ptr<Component::VBO>(new Component::VBO(id)));
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
	return randWithin(-range * 0.5f + offset, range * 0.5f + offset);
}
float TerrainSystem::BiomeDeviation(float biome, float continent) {
	return Gaussian(biome, m_biomeAmplitude, m_biomeShift, m_biomeWidth) * Sigmoid(continent, 1.f, m_continentShift, m_continentWidth);
}
float TerrainSystem::Gaussian(float x, float a, float b, float c) {
	return a * exp(-pow((x - b) / (2.f*c), 2));
}
float TerrainSystem::Sigmoid(float x, float a, float b, float c) {
	return a / (1.f + exp(-c * (x - b)));
}
void TerrainSystem::Erosion() {
	const float depositK = 0.25f;
	const float erodeK = 1.f;
	const float frictionK = 0.05;
	struct Water {
		Vector3 KE;
		Vector3 deltaKE;
		float water;
		float deltaW;
		float soil;
		float deltaS;
		float lowestNeighbor;

		Water() {
			KE = Vector3(0.f, 0.f, 0.f);
			deltaKE = Vector3(0.f, 0.f, 0.f);
			water = 1.f;
			deltaW = 0.f;
			soil = 0.f;
			deltaS = 0.f;
		}
	};
	// create the water map
	vector< vector<Water> > waterMap = vector< vector<Water> >(m_terrain.width + 1, vector<Water>(m_terrain.width + 1));
	// update
	for (int iteration = 0; iteration < 5; iteration++) {
		for (int x = 0; x <= m_terrain.width; x++) {
			for (int y = 0; y <= m_terrain.width; y++) {
				float volume = waterMap[x][y].water + waterMap[x][y].soil;
				float deltaW = 0.f;
				float deltaS = 0.f;
				// add valid ajacent cells to calculation
				waterMap[x][y].lowestNeighbor = m_terrain.map[x][y];
				for (int rot = 0; rot < 4; rot++) {
					int adjX = x + round(cos(rot * XM_PI / 2));
					int adjY = y + round(sin(rot * XM_PI / 2));
					if (adjX >= 0 && adjX <= m_terrain.width && adjY >= 0 && adjY <= m_terrain.width) {
						float adjVolume = waterMap[adjX][adjY].water + waterMap[adjX][adjY].soil;
						if (m_terrain.map[adjX][adjY] < waterMap[x][y].lowestNeighbor) {
							waterMap[x][y].lowestNeighbor = m_terrain.map[adjX][adjY];
						}
						float difference = (m_terrain.map[adjX][adjY] + adjVolume) - (m_terrain.map[x][y] + volume);
						float transferVolume = std::max(-volume / 4, std::min(difference / 4, adjVolume / 4));
						// material transfer
						float overlap;
						if (transferVolume > 0) {
							deltaW += (waterMap[adjX][adjY].water / adjVolume) * transferVolume;
							deltaS += (waterMap[adjX][adjY].soil / adjVolume) * transferVolume;
							overlap = m_terrain.map[x][y] + volume - m_terrain.map[adjX][adjY];
						}
						else {
							deltaW += (waterMap[x][y].water / volume) * transferVolume;
							deltaS += (waterMap[x][y].soil / volume) * transferVolume;
							overlap = m_terrain.map[adjX][adjY] + adjVolume - m_terrain.map[x][y];
						}
						// soil diffusion
						if (overlap > 0) {
							deltaS += (((waterMap[adjX][adjY].soil / adjVolume) * overlap) + ((waterMap[x][y].soil / volume) * overlap)) / 2 - ((waterMap[x][y].soil / volume) * overlap);
						}
						// add gravitational and kinetic energy
						if (abs(transferVolume) > 0) {
							if (adjY == y) {
								waterMap[x][y].deltaKE.x += (transferVolume * 9.8 * difference) + (transferVolume > 0 ? waterMap[adjX][adjY].KE.x * (transferVolume / (waterMap[adjX][adjY].water + waterMap[adjX][adjY].soil)) : waterMap[x][y].KE.x * (transferVolume / (waterMap[x][y].water + waterMap[x][y].soil)));
							}
							if (adjX == x) {
								waterMap[x][y].deltaKE.z += (transferVolume * 9.8 * difference) + (transferVolume > 0 ? waterMap[adjX][adjY].KE.z * (transferVolume / (waterMap[adjX][adjY].water + waterMap[adjX][adjY].soil)) : waterMap[x][y].KE.z * (transferVolume / (waterMap[x][y].water + waterMap[x][y].soil)));
							}
						}
					}
				}
				// set final delta values
				waterMap[x][y].deltaW += deltaW;
				waterMap[x][y].deltaS += deltaS;
			}
		}
		for (int x = 0; x <= m_terrain.width; x++) {
			for (int y = 0; y <= m_terrain.width; y++) {
				// evaporation
				waterMap[x][y].water *= 0.75;
				// add surface water
				waterMap[x][y].water += 1;
				// update cell
				waterMap[x][y].KE += waterMap[x][y].deltaKE;
				waterMap[x][y].deltaKE = Vector3(0.f, 0.f, 0.f);
				waterMap[x][y].water += waterMap[x][y].deltaW;
				waterMap[x][y].deltaW = 0;
				waterMap[x][y].soil += waterMap[x][y].deltaS;
				waterMap[x][y].deltaS = 0;
				// loss of energy
				waterMap[x][y].KE *= frictionK;
				// update terrain
				if (waterMap[x][y].water > 0) {
					float volume = waterMap[x][y].water + waterMap[x][y].soil;
					float soil = 0.f;
					Vector3 velocity(sqrt((2 * waterMap[x][y].KE.x) / volume), 0.f, sqrt((2 * waterMap[x][y].KE.z) / volume));
					float speed = velocity.Length();

					// erode
					soil = Sigmoid(speed, (waterMap[x][y].lowestNeighbor - m_terrain.map[x][y]) * erodeK, 0.f, 5.f);
					m_terrain.map[x][y] += soil;
					waterMap[x][y].soil -= soil;
					// deposit
					//if (velocity < 1.f && waterMap[x][y].soil > 3) {
					soil = (waterMap[x][y].soil * depositK) - Sigmoid(speed, waterMap[x][y].soil * depositK, 0.f, 5.f);
					m_terrain.map[x][y] += soil;
					waterMap[x][y].soil -= soil;
					//}
				}
			}
		}
	}
}