#pragma once

#include "Utility.h"
#include "Player.h"
#include "Region.h"
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include "CircularArray.h"

using namespace DirectX;
using namespace Utility;
using namespace std;

class World {
public:
	//--------------------------------------------------------
	// constructors

	World();
	~World();
	
	//--------------------------------------------------------
	// Loading world components

	void Initialize(ID3D11Device * device);
	// generator
	void CreateWorld(int seed, string name);
	void CreatePlayer(string name);

	// loading from files
	void LoadWorld(string name);
	void FillRegions();
	void LoadPlayer();

	//--------------------------------------------------------
	// Game Loop
	void Update(float elapsed,DirectX::Mouse::State mouse, DirectX::Keyboard::State keyboard);
	void Render();
	shared_ptr<CircularArray> GetRegions();
	// Player
	Player * GetPlayer();
	
private:
	ID3D11Device * m_device;
	//--------------------------------------------------------
	// updating buffers

	// vertex Buffer size (in number of elements)
	unsigned int entityVBSize;
	unsigned int terrainVBSize;
	unsigned int objectVBSize;
	// index Buffer size (in number of elements)
	unsigned int entityIBSize;
	unsigned int terrainIBSize;
	unsigned int objectIBSize;

	//--------------------------------------------------------
	// Loading regions
	shared_ptr<CircularArray> m_regions;
	/* loadRegions() is called by game loop, decides which loading method is best based on
	the players movement and currently loaded regions*/
	void LoadRegions();
	
	float m_loadWidth;

	//--------------------------------------------------------
	// World constants

	string			m_name;
	unsigned int	m_worldWidth;
	unsigned int	m_regionWidth;
	unsigned int	m_regionSize;
	int				m_seed;

	//--------------------------------------------------------
	// Player

	unique_ptr<Player> m_player;
	unsigned short m_lastX = 0;
	unsigned short m_lastZ = 0;

	//--------------------------------------------------------
	// Collision

	float yOnABC(float x, float y, XMFLOAT3 A, XMFLOAT3 B, XMFLOAT3 C);
	XMFLOAT3 globalToRegionCoord(XMFLOAT3 position);
};

