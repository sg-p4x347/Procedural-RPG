#include "pch.h"
#include "World.h"
#include "JsonParser.h"
#include "Utility.h"
#include "EntityManager.h"
#include <iostream>
#include "Filesystem.h"
#include "TerrainSystem.h"

using namespace DirectX::SimpleMath;
using namespace std;

World::World(
	string directory,
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
	std::shared_ptr<DirectX::CommonStates> states,
	std::shared_ptr<DirectX::Mouse> mouse,
	std::shared_ptr<DirectX::Keyboard> keyboard
) : m_directory(directory) {
	m_systemManager = std::make_unique<SystemManager>(SystemManager(m_directory,device,context,states,mouse,keyboard));
	CreateDevice(device);
	m_d3dDevice = device;
	JsonParser config(std::ifstream("config/continent.json"));
	m_worldWidth = config["terrainMap"]["width"].To<int>();
	m_regionWidth = 512;
	m_loadWidth = 2;
	m_NG = shared_ptr<NameGenerator>(new NameGenerator());
}
World::World(const string directory) : m_directory(directory)
{

}
// creates a new world
void World::Generate(int seed) {
	// Create the world directory
	Filesystem::create_directory(m_directory);

	// seed the RNG
	srand(seed);

	// generators

	((TerrainSystem*)m_systemManager->m_systems["Terrain"].get())->Generate();
	//shared_ptr<Continent> terrain = world->CreateTerrain(worldDir);
	//world->CreateCities(terrain);
	//GenerateHistory(m_cities);
	//world->CreatePlayer();
	
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
	//LoadPlayer();
	//LoadCities(directory + '/' + name + "/cities");
	//FillRegions();
}
void World::LoadRegions() {
	
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

	DX::ThrowIfFailed(device->CreateInputLayout(
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

void World::CreateResources(unsigned int backBufferWidth, unsigned int backBufferHeight, SimpleMath::Matrix & projMatrix)
{
	/*m_effect->SetViewport(float(backBufferWidth), float(backBufferHeight));

	m_effect->SetView(GetPlayer()->getViewMatrix());
	m_effect->SetProjection(projMatrix);*/
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
World::~World() {
	
}