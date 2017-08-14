#include "pch.h"
#include "World.h"
#include "JsonParser.h"
#include "Utility.h"
#include "EntityManager.h"
#include <iostream>
#include <filesystem>

using namespace DirectX::SimpleMath;
using namespace std;

World::World(
	const string directory,
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
	std::shared_ptr<DirectX::CommonStates> states,
	DirectX::Mouse::State mouse,
	DirectX::Keyboard::State keyboard
) : m_directory(directory) {
	m_systemManager = std::make_unique<SystemManager>(SystemManager(m_directory,device,context,states,mouse,keyboard));
}
void World::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> device) {
	
	m_d3dDevice = device;
	JsonParser config(std::ifstream("config/continent.json"));
	m_worldWidth = config["terrainMap"]["width"].To<int>();
	m_regionWidth = 512;
	m_loadWidth = 2;
	m_NG = shared_ptr<NameGenerator>(new NameGenerator());
	m_regions = shared_ptr<CircularArray>(new CircularArray());
	//m_entityManager = unique_ptr<EntityManager>(new EntityManager());
	m_regions->init(m_loadWidth, m_loadWidth);
	
}
// creates a new world
unique_ptr<World> World::CreateWorld(int seed,string directory, string name) {
	string worldDir = directory + '/' + name;
	unique_ptr<World> world(new World(worldDir));
	world->m_name = name;
	
	CreateFolder(worldDir);
	// create the relative path
	// create the save directory
	// CreateDirectory(wstring(m_path.begin(), m_path.end()).c_str(), NULL);

	// seed the RNG
	srand(seed);

	// generators
	shared_ptr<Continent> terrain = world->CreateTerrain(worldDir);
	world->CreateCities(terrain);
	//GenerateHistory(m_cities);
	world->CreatePlayer();
	return world;
	
}
shared_ptr<Continent> World::CreateTerrain(string directory)
{
	// generate the heightmap
	OutputDebugString(L"Generating heightmap...");
	Continent continent(directory);
	return std::make_shared<Continent>(continent);
	OutputDebugString(L"Finished!\n");
}
void World::CreateCities(shared_ptr<Continent> Continent)
{
	OutputDebugString(L"Generating cities...");
	JsonParser continentCfg(std::ifstream("config/continent.json"));
	JsonParser cityCfg(std::ifstream("config/city.json"));
	int maxCities = cityCfg["maxCities"].To<int>();
	double maxElevation = cityCfg["maxElevation"].To<double>();
	double minDistance = cityCfg["minDistance"].To<double>();
	int areaWidth = cityCfg["areaWidth"].To<int>();

	int i = 0;
	while (i < maxCities) {
	newCity:
		Rectangle areaRect = Rectangle(randWithin(0, Continent->GetWidth() - areaWidth), randWithin(0, Continent->GetWidth() - areaWidth), areaWidth, areaWidth);
		// TEMP
		City city = City(areaRect, Continent->GetTerrain(), Continent->GetBiome());
		m_cities.push_back(city);
		i++;
		continue;
		//------------
		
		Vector2 pos = areaRect.Center();
		// check elevation
		float elevation = Continent->GetTerrain().map[(int)pos.x][(int)pos.y];
		if (elevation > 0 && elevation < maxElevation) {
			// check to see if the average of the four corners is close to the center
			// this ensures that the terrain is not too rough
			double sum = 0;
			sum += Continent->GetTerrain().map[areaRect.x][areaRect.y];
			sum += Continent->GetTerrain().map[areaRect.x + areaRect.width][areaRect.y];
			sum += Continent->GetTerrain().map[areaRect.x][areaRect.y + areaRect.height];
			sum += Continent->GetTerrain().map[areaRect.x + areaRect.width][areaRect.y + areaRect.height];
			sum /= 4;
			if (abs(Continent->GetTerrain().map[pos.x][pos.y] - sum) > 10.0) {
				goto newCity;
			}

			// check proximity to all other cities
			for (unsigned int j = 0; j < m_cities.size(); j++) {
				if (abs(m_cities[j].GetPosition().x - pos.x) <= minDistance, abs(m_cities[j].GetPosition().y - pos.y) <= minDistance) {
					goto newCity;
				}
			}

			City city = City(areaRect, Continent->GetTerrain(), Continent->GetBiome());
			m_cities.push_back(city);
			i++;
		}
	}
	OutputDebugString(L"Finished!\n");
	
}
void World::LoadCities(string directory)
{
	for (auto & p : std::experimental::filesystem::directory_iterator(directory)) {
		m_cities.push_back(City(JsonParser(std::ifstream(p.path()))));
	}
}
void World::GenerateHistory(vector<City> cities)
{
	m_federal = new Federal(cities);
	m_federal->Update();
}
void World::CreatePlayer() {
	
}
void World::LoadWorld(string directory, string name) {
	m_name = name;
	// load assets
	LoadPlayer();
	LoadCities(directory + '/' + name + "/cities");
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
				// Initialize the new region into old one's place
				Region* region = m_regions->get(x, z);
				region->Initialize(
					m_d3dDevice.Get(),
					regionX - int(round(m_loadWidth / 2.f)) + x,
					regionZ - int(round(m_loadWidth / 2.f)) + z,
					m_worldWidth, m_regionWidth, m_name,
					BuildingsInRegion(region->GetArea())
				);
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
				Region* region = m_regions->get(x, z);
				region->Initialize(
					m_d3dDevice.Get(),
					regionX - int(round(m_loadWidth / 2.f)) + x,
					regionZ - int(round(m_loadWidth / 2.f)) + z,
					m_worldWidth, m_regionWidth, m_name,
					BuildingsInRegion(region->GetArea())
				);
			}
		}
	} else {
		// load a whole new set of regions and reset the circular array
		FillRegions();
	}
}
vector<shared_ptr<Architecture::Building>> World::BuildingsInRegion(const Rectangle & regionArea)
{
	vector<shared_ptr<Architecture::Building>> buildings;
	// for each city
	for (City & city : m_cities) {
		if (city.GetArea().Intersects(regionArea)) {
			// for each buliding
			for (shared_ptr<Architecture::Building> & building : city.GetBuildings()) {
				if (regionArea.Contains(building->GetFootprint().Center())) {
					// this building is in the region
					buildings.push_back(building);
				}
			}
		}
	}
	return buildings;
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
			Region* region = m_regions->get(x, z);
			region->Initialize(
				m_d3dDevice.Get(),
				regionX - int(round(m_loadWidth / 2.f)) + x,
				regionZ - int(round(m_loadWidth / 2.f)) + z,
				m_worldWidth, m_regionWidth, m_name,
				BuildingsInRegion(region->GetArea())
			);
			index++;
		}
	}
}

XMFLOAT3 World::globalToRegionCoord(XMFLOAT3 position) {
	return XMFLOAT3(float(int(floor(position.x)) % (m_regionWidth+1)), position.y, float(int(floor(position.x)) % (m_regionWidth+1)));
}
void World::Import(JsonParser & jp)
{
}
JsonParser World::Export()
{
	return JsonParser();
}
void World::CreateDevice(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	// Create DGSL Effect
	auto blob = DX::ReadData(L"Terrain.cso"); // .cso is the compiled version of the hlsl shader (compiled shader object)
	DX::ThrowIfFailed(device->CreatePixelShader(&blob.front(), blob.size(),
		nullptr, m_pixelShader.ReleaseAndGetAddressOf()));

	m_effect = std::make_unique<DGSLEffect>(device.Get(), m_pixelShader.Get());
	m_effect->SetTextureEnabled(true);
	m_effect->SetVertexColorEnabled(true);
	//---Textures---
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device.Get(), L"dirt.dds", nullptr,
			m_texture.ReleaseAndGetAddressOf()));

	m_effect->SetTexture(m_texture.Get());

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device.Get(), L"grass.dds", nullptr,
			m_texture2.ReleaseAndGetAddressOf()));

	m_effect->SetTexture(1, m_texture2.Get());

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device.Get(), L"stone.dds", nullptr,
			m_texture3.ReleaseAndGetAddressOf()));

	m_effect->SetTexture(2, m_texture3.Get());

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device.Get(), L"snow.dds", nullptr,
			m_texture4.ReleaseAndGetAddressOf()));

	m_effect->SetTexture(3, m_texture4.Get());
	//---Textures---

	m_effect->EnableDefaultLighting();

	void const* shaderByteCode;
	size_t byteCodeLength;

	m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	DX::ThrowIfFailed(m_d3dDevice->CreateInputLayout(
		VertexPositionNormalTangentColorTexture::InputElements,
		VertexPositionNormalTangentColorTexture::InputElementCount,
		shaderByteCode, byteCodeLength,
		m_inputLayout.ReleaseAndGetAddressOf()));

	// Matricies
	m_worldMatrix = Matrix::Identity;

}
float World::yOnABC(float x, float z, XMFLOAT3 A, XMFLOAT3 B, XMFLOAT3 C) {
	XMFLOAT3 u = XMFLOAT3(B.x - A.x, B.y - A.y, B.z - A.z);
	XMFLOAT3 v = XMFLOAT3(C.x = A.x, C.y - A.y, C.z - A.z);
	XMFLOAT3 N = XMFLOAT3(-u.y*v.z + u.z*v.y, -u.z*v.x + u.x*v.z, -u.x*v.y + u.y*v.x);
	XMFLOAT3 V = XMFLOAT3(A.x - x, 0, A.z - z);
	return A.y + ((N.z * V.z) + (N.x * V.x)) / N.y;

}
void World::Update(
	float elapsed,
	DirectX::Mouse::State mouse,
	DirectX::Keyboard::State keyboard
) {
	// update the player's physics
	m_player->update(elapsed, mouse, keyboard);
	//m_player->updatePhysics(elapsed);
	// load the regions around the player's position

	//initialize thread0data
	LoadRegions();
}
void World::Render(
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
	std::shared_ptr<DirectX::CommonStates> states
) {
	

	

	//TODO: Add your rendering code here.
	m_effect->Apply(context.Get());

	// camera
	m_effect->SetView(m_player->getViewMatrix());

	// Render regions
	auto sampler = states->LinearWrap();

	context->PSSetSamplers(0, 1, &sampler);
	context->RSSetState(states->CullCounterClockwise());
	context->IASetInputLayout(m_inputLayout.Get());

	for (int i = m_regions->size() - 1; i >= 0; i--) {
		if (!m_regions->data[i].IsNull()) {
			m_regions->data[i].Render(context, m_batch.get());
		}
	}

	
}

void World::CreateResources(unsigned int backBufferWidth, unsigned int backBufferHeight, DirectX::XMMATRIX projMatrix)
{
	m_effect->SetViewport(float(backBufferWidth), float(backBufferHeight));

	m_effect->SetView(GetPlayer()->getViewMatrix());
	m_effect->SetProjection(projMatrix);
}
void World::OnDeviceLost()
{
	m_effect.reset();
	m_inputLayout.Reset();
	m_texture.Reset();
	m_texture2.Reset();
	m_texture3.Reset();
	m_pixelShader.Reset();
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
	std::ifstream playerFile("saves/" + m_name + "/player.bin", ios::binary);
	if (playerFile.is_open()) {
		// read the file into the player class
		m_player = make_unique<Player>(XMFLOAT3(0,32,0), 100.0f);
		m_lastX = int(floor(m_player->getPosition().x / float(m_regionWidth)));
		m_lastZ = int(floor(m_player->getPosition().z / float(m_regionWidth)));
	}
	playerFile.close();
}
void World::SaveWorld(string directory)
{
	// Cities
	string citiesDir = directory + '/' + m_name + "/cities";
	CreateFolder(citiesDir);
	for (City & city : m_cities) {
		city.SaveCity(citiesDir);
	}
}
World::~World() {
	
}