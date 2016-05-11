#pragma once

#include "ShaderStructures.h"
#include "Utility.h"
#include "Player.h"
#include "Region.h"
#include "pch.h"
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include "CircularArray.h"
#include "d3dclass.h"
#include "ShaderManager.h"
#include "inputclass.h"

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

	void init(unsigned int worldWidthIn, unsigned int regionWidthIn, unsigned int loadWidthIn, string workingPathIn);
	void fillRegions(ID3D11Device* device);
	void loadPlayer();

	//--------------------------------------------------------
	// Game Loop
	void update(ID3D11Device* device, InputClass * input,double elapsed);
	void render(D3DClass* Direct3D, ShaderManager* shaderManager);

	
private:
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
	CircularArray * regions;
	/* gets called by game loop, decides which loading method is best based on
	the players movement, and currently loaded regions*/
	void loadRegions(ID3D11Device* device);
	
	//void loadRegion(int regionX, int regionZ);
	void unloadRegion(int regionX, int regionZ);
	void saveRegion(int regionX, int regionZ);
	unsigned short loadWidth;

	//--------------------------------------------------------
	// World constants

	unsigned int worldWidth;
	unsigned int regionWidth;
	string workingPath;
	unsigned int regionSize;
	int seed;

	//--------------------------------------------------------
	// Storing regions

	
	unsigned short lastX = 0;
	unsigned short lastZ = 0;

	//--------------------------------------------------------
	// Player

	Player player = Player(XMFLOAT3(128, 64, 128), 100.0f);

	//--------------------------------------------------------
	// Collision

	float yOnABC(float x, float y, XMFLOAT3 A, XMFLOAT3 B, XMFLOAT3 C);
	XMFLOAT3 globalToRegionCoord(XMFLOAT3 position);
};

