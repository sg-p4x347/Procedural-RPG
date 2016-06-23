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

	void init(ID3D11Device * device,unsigned int worldWidthIn, unsigned int regionWidthIn, float loadWidthIn, string workingPathIn);
	void fillRegions();
	void loadPlayer();

	//--------------------------------------------------------
	// Game Loop
	void update(float elapsed,DirectX::Mouse::State mouse, DirectX::Keyboard::State keyboard);
	void render();
	shared_ptr<CircularArray> getRegions();
	// Player
	Player* getPlayer();
	
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
	/* gets called by game loop, decides which loading method is best based on
	the players movement, and currently loaded regions*/
	void loadRegions();
	
	float m_loadWidth;

	//--------------------------------------------------------
	// World constants

	unsigned int m_worldWidth;
	unsigned int m_regionWidth;
	string m_workingPath;
	unsigned int m_regionSize;
	int m_seed;

	//--------------------------------------------------------
	// Storing regions

	
	unsigned short m_lastX = 0;
	unsigned short m_lastZ = 0;

	//--------------------------------------------------------
	// Player

	unique_ptr<Player> m_player;

	//--------------------------------------------------------
	// Collision

	float yOnABC(float x, float y, XMFLOAT3 A, XMFLOAT3 B, XMFLOAT3 C);
	XMFLOAT3 globalToRegionCoord(XMFLOAT3 position);
};

