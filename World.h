#pragma once

#include "Utility.h"
#include "Player.h"
#include "Region.h"
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include "CircularArray.h"
#include "NameGenerator.h"
#include "Distribution.h"
#include "City.h"
#include "EntityManager.h"

using namespace DirectX;
using namespace Utility;
using namespace std;

class World {
public:
	//--------------------------------
	// constructors

	World();
	~World();
	
	//--------------------------------
	// Loading world components

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> device);
	// generator
	void CreateWorld(int seed, string name);
	shared_ptr<Distribution> CreateTerrain();
	void CreateCities(shared_ptr<Distribution> terrain);
	void CreatePlayer();

	// loading from files
	void LoadWorld(string name);
	void FillRegions();
	void LoadPlayer();

	//--------------------------------
	// Game Loop
	void Update(float elapsed,DirectX::Mouse::State mouse, DirectX::Keyboard::State keyboard);
	void Render();
	void CreateResources(unsigned int backBufferWidth, unsigned int backBufferHeight);
	shared_ptr<CircularArray> GetRegions();
	// Player
	Player * GetPlayer();
	
private:
	//--------------------------------
	// DirectX
	Microsoft::WRL::ComPtr<ID3D11Device>		m_d3dDevice;

	// name generator
	shared_ptr<NameGenerator> m_NG;

	//--------------------------------
	// Loading regions
	shared_ptr<CircularArray> m_regions;
	/* loadRegions() is called by game loop, decides which loading method is best based on
	the players movement and currently loaded regions*/
	void LoadRegions();
	
	int m_loadWidth;

	//--------------------------------
	// World constants

	string			m_name;
	string			m_path;
	unsigned int	m_worldWidth;
	unsigned int	m_regionWidth;
	int				m_regionSize;
	int				m_seed;

	//--------------------------------
	// Cities

	vector<City>	m_cities;

	//--------------------------------
	// Player

	unique_ptr<Player> m_player;
	int m_lastX = 0;
	int m_lastZ = 0;

	//--------------------------------
	// entities
	unique_ptr<EntityManager> m_entityManager;


	//--------------------------------
	// Collision

	float yOnABC(float x, float y, XMFLOAT3 A, XMFLOAT3 B, XMFLOAT3 C);
	XMFLOAT3 globalToRegionCoord(XMFLOAT3 position);
};

