#pragma once

#include "Utility.h"
//#include "Player.h"
#include "Region.h"

#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include "CircularArray.h"
#include "NameGenerator.h"
#include "Continent.h"
#include "City.h"
#include "Building.h"
#include "SystemManager.h"
#include "JSON.h"
#include "Federal.h"

using namespace DirectX;
using namespace Utility;
using namespace std;

class World : public JSON {
public:
	//--------------------------------
	// constructors

	World(
		string directory,
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		std::shared_ptr<DirectX::CommonStates> states, 
		std::shared_ptr<DirectX::Mouse> mouse,
		std::shared_ptr<DirectX::Keyboard> keyboard
	);
	World(
		const string directory
	);
	~World();
	
	//--------------------------------
	// Loading world components

	// generator
	void Generate(int seed);
	shared_ptr<Continent> CreateTerrain(string directory);


	void CreatePlayer();

	// loading from files
	void LoadWorld(string directory, string name);
	//void FillRegions();
	//void LoadPlayer();
	// saving to files
	//void SaveWorld(string directory);
	//--------------------------------
	// Game Loop
	void CreateDevice(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void CreateResources(unsigned int backBufferWidth, unsigned int backBufferHeight, SimpleMath::Matrix & prjMatrix);
	void OnDeviceLost();
	
	// Inherited via JSON
	virtual void Import(JsonParser & jp) override;
	virtual JsonParser Export() override;
private:
	//--------------------------------
	// DirectX
	Microsoft::WRL::ComPtr<ID3D11Device>									m_d3dDevice;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>	m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>								m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>								m_pixelShader;
	// Matricies
	DirectX::SimpleMath::Matrix	m_worldMatrix;
	

	std::unique_ptr<DirectX::DGSLEffect>		m_effect;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture3;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture4;

	
	

	// name generator
	shared_ptr<NameGenerator> m_NG;

	//--------------------------------
	// Loading regions
	

	/* loadRegions() is called by game loop, decides which loading method is best based on
	the players movement and currently loaded regions*/
	void LoadRegions();
	
	int m_loadWidth;
	//----------------------------------------------------------------
	// Loading Buildings into regions
	vector<shared_ptr<Architecture::Building>> BuildingsInRegion(const Rectangle & regionArea);

	//--------------------------------
	// World constants
	const string	m_directory;
	string			m_name;
	unsigned int	m_worldWidth;
	unsigned int	m_regionWidth;
	int				m_regionSize;
	int				m_seed;

	// Utility
	inline Vector2 RegionToWorld(int regionX, int regionZ) {
		return Vector2((float)regionX*m_regionWidth, (float)regionZ*m_regionWidth);
	}

	//----------------------------------------------------------------
	// History / government
	void GenerateHistory(vector<City> cities);
	Federal * m_federal; // federal division of organization (contains states)

	//----------------------------------------------------------------
	// Cities
	void CreateCities(shared_ptr<Continent> terrain);
	void LoadCities(string directory);
	vector<City>	m_cities;

	//----------------------------------------------------------------
	// Player

	int m_lastX = 0;
	int m_lastZ = 0;

	//--------------------------------
	// entities
	unique_ptr<SystemManager> m_systemManager;

	//--------------------------------
	// Collision

	float yOnABC(float x, float y, XMFLOAT3 A, XMFLOAT3 B, XMFLOAT3 C);
	XMFLOAT3 globalToRegionCoord(XMFLOAT3 position);

	
};

