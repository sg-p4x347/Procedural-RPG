#include "pch.h"
#include "World.h"
#include "JsonParser.h"
#include "Utility.h"

using namespace DirectX::SimpleMath;
using namespace std;

World::World() {
}
void World::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> device) {
	
	m_d3dDevice = device;
	JsonParser config(ifstream("config/continent.json"));
	m_worldWidth = config["terrainMap"]["width"].To<int>();
	m_regionWidth = 512;
	m_loadWidth = 2;
	m_NG = shared_ptr<NameGenerator>(new NameGenerator());
	m_regions = shared_ptr<CircularArray>(new CircularArray());
	//m_entityManager = unique_ptr<EntityManager>(new EntityManager());
	m_regions->init(m_loadWidth, m_loadWidth);

}
// creates a new world file
void World::CreateWorld(int seed, string name) {
	m_name = name;
	// create the relative path
	m_path = "saves/" + name + "/";
	// create the save directory
	CreateDirectory(wstring(m_path.begin(), m_path.end()).c_str(), NULL);

	// seed the RNG
	srand(seed);

	// generators
	CreateTerrain();
	/*shared_ptr<Continent> terrain = CreateTerrain();
	CreateCities(terrain);*/
	CreatePlayer();
	
}
void World::CreateTerrain()
{
	// generate the heightmap
	OutputDebugString(L"Generating heightmap...");
	Continent continent(m_path);
	OutputDebugString(L"Finished!\n");
}
void World::CreatePlayer() {
	
}
void World::LoadWorld(string name) {
	m_name = name;
	// load assets
	LoadPlayer();
	FillRegions();
}
void World::LoadRegions() {
	// calculate displacement
	int regionX = int(round(m_player->getPosition().x / float(m_regionWidth)));
	int regionZ = int(round(m_player->getPosition().z / float(m_regionWidth)));
	// get the region displacement
	int displacementX = regionX - m_lastX;
	int displacementZ = regionZ - m_lastZ;
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
				m_regions->set(x, z, m_d3dDevice.Get(), regionX - int(round(m_loadWidth / 2.f)) + x, regionZ - int(round(m_loadWidth / 2.f)) + z, m_worldWidth, m_regionWidth, m_name);
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
				m_regions->set(x, z, m_d3dDevice.Get(), regionX - int(round( m_loadWidth / 2.f)) + x, regionZ - int(round(m_loadWidth / 2.f)) + z, m_worldWidth, m_regionWidth, m_name);
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
	int regionZ = int(round(m_player->getPosition().z / m_regionWidth));
	int regionX = int(round(m_player->getPosition().x / m_regionWidth));
	// load a grid of regions around the player; loadWidth is the width and height in regions
	for (int z = 0; z < m_loadWidth; z++) {
		for (int x = 0; x < m_loadWidth; x++) {
			// load a new region for updating
			m_regions->set(x, z, m_d3dDevice.Get(), regionX - int(round(m_loadWidth / 2.f)) + x, regionZ - int(round(m_loadWidth / 2.f)) + z, m_worldWidth, m_regionWidth, m_name);
			index++;
		}
	}
}
void World::Update(float elapsed, DirectX::Mouse::State mouse, DirectX::Keyboard::State keyboard) {
	// update the player's physics
	m_player->update(elapsed,mouse,keyboard);
	//m_player->updatePhysics(elapsed);
	// load the regions around the player's position

	//initialize thread0data
	LoadRegions();
}
XMFLOAT3 World::globalToRegionCoord(XMFLOAT3 position) {
	return XMFLOAT3(float(int(floor(position.x)) % (m_regionWidth+1)), position.y, float(int(floor(position.x)) % (m_regionWidth+1)));
}
float World::yOnABC(float x, float z, XMFLOAT3 A, XMFLOAT3 B, XMFLOAT3 C) {
	XMFLOAT3 u = XMFLOAT3(B.x - A.x, B.y - A.y, B.z - A.z);
	XMFLOAT3 v = XMFLOAT3(C.x = A.x, C.y - A.y, C.z - A.z);
	XMFLOAT3 N = XMFLOAT3(-u.y*v.z + u.z*v.y, -u.z*v.x + u.x*v.z, -u.x*v.y + u.y*v.x);
	XMFLOAT3 V = XMFLOAT3(A.x - x, 0, A.z - z);
	return A.y + ((N.z * V.z) + (N.x * V.x)) / N.y;

}
void World::Render() {
	

}
void World::CreateResources(unsigned int backBufferWidth, unsigned int backBufferHeight)
{
	
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
		m_player = make_unique<Player>(XMFLOAT3(0,32,0), 100.0f);
		m_lastX = int(floor(m_player->getPosition().x / float(m_regionWidth)));
		m_lastZ = int(floor(m_player->getPosition().z / float(m_regionWidth)));
	}
	playerFile.close();
}
World::~World() {
	
}