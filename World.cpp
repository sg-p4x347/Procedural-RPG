#include "pch.h"
#include "World.h"
#include "Distribution.h"

using namespace DirectX::SimpleMath;

World::World() {
}
void World::Initialize(ID3D11Device * device) {
	
	m_device = device;
	m_worldWidth = 512;
	m_regionWidth = 64;
	m_loadWidth = 8;
	m_regions = unique_ptr<CircularArray>(new CircularArray());
	m_regions->init(ceil(m_loadWidth), ceil(m_loadWidth));
	
}
// creates a new world file
void World::CreateWorld(int seed, string name) {
	// create the relative path
	string workingPath = "saves/" + name + "/";
	// create the save directory
	CreateDirectory(wstring(workingPath.begin(), workingPath.end()).c_str(), NULL);

	// seed the RNG
	srand(seed);
	// generate the terrain distribution maps
	unique_ptr<Distribution> continentDist(new Distribution(m_worldWidth, m_worldWidth));
	continentDist->Continent(2);
	/*continentDist->DiamondSquare(m_worldWidth / 8, 0.55f, 2, true);
	continentDist->Erosion();*/
	//continentDist->DiamondSquare(m_worldWidth / 16, 1.0f, 3, false);
	vector< vector<float> > terrain = continentDist->GetPoints();

	unsigned int vertexCount = (m_regionWidth + 1)*(m_regionWidth + 1);
	unsigned int regionCount = (continentDist->GetWidth() / m_regionWidth)*(continentDist->GetWidth() / m_regionWidth);
	
	//-------------------------------
	// save terrain to binary file
	//-------------------------------
	
	unsigned char *terrainBuffer = new unsigned char[vertexCount*regionCount * sizeof(short)];
	char *normalBuffer = new char[vertexCount*regionCount * 3]; // X (8 bits) Y (8 bits) Z (8 bits)

	unsigned int index = 0;
	// push each vertex in the world
	for (unsigned short regionZ = 0; regionZ < m_worldWidth / m_regionWidth; regionZ++) {
		for (unsigned short regionX = 0; regionX < m_worldWidth / m_regionWidth; regionX++) {
			for (unsigned short vertZ = 0; vertZ <= m_regionWidth; vertZ++) {
				for (unsigned short vertX = 0; vertX <= m_regionWidth; vertX++) {
					float vertex = terrain[regionX * m_regionWidth + vertX][regionZ * m_regionWidth + vertZ];
					terrainBuffer[index * 2] = short(vertex * 10) & 0xff;
					terrainBuffer[index * 2 + 1] = (short(vertex * 10) >> 8) & 0xff;

					// normals
					float left = signed(regionX * m_regionWidth + vertX) - 1 >= 0 ? terrain[regionX * m_regionWidth + vertX - 1][regionZ * m_regionWidth + vertZ] : vertex;
					float right = signed(regionX * m_regionWidth + vertX) + 1 <= signed(m_worldWidth) ? terrain[regionX * m_regionWidth + vertX + 1][regionZ * m_regionWidth + vertZ] : vertex;
					float up = signed(regionZ * m_regionWidth + vertZ) + 1 <= signed(m_worldWidth) ? terrain[regionX * m_regionWidth + vertX][regionZ * m_regionWidth + vertZ + 1] : vertex;
					float down = signed(regionZ * m_regionWidth + vertZ) - 1 >= 0 ? terrain[regionX * m_regionWidth + vertX][regionZ * m_regionWidth + vertZ - 1] : vertex;

					Vector3 normal = Vector3(left - right, 2.f, down - up);
					normal.Normalize();
					// scale the vector up into the characters range
					normal *= 128;
					normalBuffer[index * 3] = char(normal.x);
					normalBuffer[index * 3 + 1] = char(normal.y);
					normalBuffer[index * 3 + 2] = char(normal.z);

					// update vertex index
					index++;
				}
			}
		}
	}
	//-----------------------------------------------
	// output file stream
	//-----------------------------------------------

	// heightMap
	ofstream terrainFile(workingPath + "terrain.bin", ios::binary);
	terrainFile.seekp(0);
	if (terrainFile.is_open()) {
		// write the data
		terrainFile.write((const char *)terrainBuffer, vertexCount*regionCount * sizeof(short));
	}
	terrainFile.close();

	// normalMap
	ofstream normalFile(workingPath + "normal.bin", ios::binary);
	normalFile.seekp(0);
	if (normalFile.is_open()) {
		// write the data
		normalFile.write((const char *)normalBuffer, vertexCount*regionCount * 3);
	}
	normalFile.close();

	delete[] terrainBuffer;
}
void World::CreatePlayer(string name) {

}
void World::LoadWorld(string name) {
	m_name = name;
	// load assets
	LoadPlayer();
	FillRegions();
}
void World::LoadRegions() {
	// calculate displacement
	int regionX = round(m_player->getPosition().x / float(m_regionWidth));
	int regionZ = round(m_player->getPosition().z / float(m_regionWidth));
	// get the region displacement
	short displacementX = regionX - m_lastX;
	short displacementZ = regionZ - m_lastZ;
	// update the position of last region
	m_lastX = regionX;
	m_lastZ = regionZ;
	if (displacementX == 0 && displacementZ == 0) {
		// don't load anything
		return;
	} else if (abs(displacementX) <= 1 && abs(displacementZ) <= 1) {
		// roll the new regions into the old one's places
		m_regions->offsetX(displacementX);
		m_regions->offsetY(displacementZ);
		if (abs(displacementX) == 1) {
			// the left or right most collumn is replaced with the new regions
			int x;
			if (displacementX == -1) {
				x = 0;
			} else if (displacementX == 1) {
				x = m_loadWidth - 1;
			}
			for (int z = 0; z < m_loadWidth; z++) {
				// place the new region into old one's place
				m_regions->set(x, z, m_device, regionX - round(m_loadWidth / 2.f) + x, regionZ - round(m_loadWidth / 2.f) + z, m_worldWidth, m_regionWidth, m_name);
			}
		}
		if (abs(displacementZ) == 1) {
			// the left or right most row is replaced with the new regions
			int z;
			if (displacementZ == -1) {
				z = 0;
			} else if (displacementZ == 1) {
				z = m_loadWidth - 1;
			}
			for (int x = 0; x < m_loadWidth; x++) {
				// place the new region into old one's place
				m_regions->set(x, z, m_device, regionX - round( m_loadWidth / 2.f) + x, regionZ - round(m_loadWidth / 2.f) + z, m_worldWidth, m_regionWidth, m_name);
			}
		}
	} else {
		// load a whole new set of regions and reset the circular array
		FillRegions();
	}
}
void World::FillRegions() {
	m_regions->zeroOffsets();
	int index = 0;
	// find the region coordinate that the player is located in
	int regionZ = round(m_player->getPosition().z / m_regionWidth);
	int regionX = round(m_player->getPosition().x / m_regionWidth);
	// load a grid of regions around the player; loadWidth is the width and height in regions
	for (int z = 0; z < m_loadWidth; z++) {
		for (int x = 0; x < m_loadWidth; x++) {
			// load a new region for updating
			m_regions->set(x, z, m_device, regionX - round(m_loadWidth / 2.f) + x, regionZ - round(m_loadWidth / 2.f) + z, m_worldWidth, m_regionWidth, m_name);
			index++;
		}
	}
}
void World::Update(float elapsed, DirectX::Mouse::State mouse, DirectX::Keyboard::State keyboard) {
	// update the player's physics
	m_player->update(elapsed,mouse,keyboard);
	//m_player->updatePhysics(elapsed);
	// load the regions around the player's position
	LoadRegions();
}
XMFLOAT3 World::globalToRegionCoord(XMFLOAT3 position) {
	return XMFLOAT3(int(floor(position.x)) % (m_regionWidth+1), position.y, int(floor(position.x)) % (m_regionWidth+1));
}
float World::yOnABC(float x, float z, XMFLOAT3 A, XMFLOAT3 B, XMFLOAT3 C) {
	XMFLOAT3 u = XMFLOAT3(B.x - A.x, B.y - A.y, B.z - A.z);
	XMFLOAT3 v = XMFLOAT3(C.x = A.x, C.y - A.y, C.z - A.z);
	XMFLOAT3 N = XMFLOAT3(-u.y*v.z + u.z*v.y, -u.z*v.x + u.x*v.z, -u.x*v.y + u.y*v.x);
	XMFLOAT3 V = XMFLOAT3(A.x - x, 0, A.z - z);
	return A.y + ((N.z * V.z) + (N.x * V.x)) / N.y;

}
void World::Render() {
	//XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;

	//// Clear the buffers to begin the scene.
	//Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	//// Generate the view matrix based on the camera's position.
	//m_player->render();

	//// Get the world, view, and projection matrices from the camera and d3d objects.
	//Direct3D->GetWorldMatrix(worldMatrix);
	//m_player->getViewMatrix(viewMatrix);
	//Direct3D->GetProjectionMatrix(projectionMatrix);
	//m_player->getBaseViewMatrix(baseViewMatrix);
	//Direct3D->GetOrthoMatrix(orthoMatrix);

	//// Render regions
	//for (int i = m_regions->size()-1; i >= 0; i--) {
	//	if (!m_regions->data[i].isNull()) {
	//		m_regions->data[i].render(Direct3D->GetDeviceContext());
	//	}
	//}
	//
	//// Present the rendered scene to the screen.
	//Direct3D->EndScene();

}
shared_ptr<CircularArray> World::GetRegions()
{
	return m_regions;
}
Player * World::GetPlayer()
{
	return m_player.get();
}
void World::LoadPlayer() {
	ifstream playerFile("saves/" + m_name + "/player.bin", ios::binary);
	if (playerFile.is_open()) {
		// read the file into the player class
		m_player = make_unique<Player>(XMFLOAT3(128,32,128), 100.0f);
		m_lastX = floor(m_player->getPosition().x / float(m_regionWidth));
		m_lastZ = floor(m_player->getPosition().z / float(m_regionWidth));
	}
	playerFile.close();
}
World::~World() {
	
}