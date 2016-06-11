#pragma once

#include "VertexTypes.h"
#include "Utility.h"
#include "Entity.h"
#include "NPC.h"
#include <fstream>
#include <iostream>
#include <string>
#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;
using namespace Utility;
using namespace std;

class Region {
	public:
		// constructors
		Region();
		~Region();
		// initialize
		void init(shared_ptr<ID3D11Device> device, int x, int z, unsigned int worldWidthIn, unsigned int regionWidthIn, string workingPathIn);
		// put buffers on graphics pipeline
		void render(ID3D11DeviceContext* deviceContext);
		vector<VertexPositionColor> vertices;
		// getters
		int getIndexCount();
		bool isNull();
	private:
		shared_ptr<ID3D11Device> m_device;
		// nullness
		bool Null = true;
		// vertex buffers
		ID3D11Buffer * terrainVB;
		// index buffers
		ID3D11Buffer * terrainIB;
		int indexCount;
		// properties
		unsigned short regionX;
		unsigned short regionZ;
		unsigned int worldWidth;
		unsigned int regionWidth;
		string workingPath;
		// load terrain into terrain buffers
		HRESULT loadTerrain();
		// load objects into object buffers
		void loadObjects();
		// load entities into the entity storage vector
		void loadEntities();
		// update entity vertices
		void updateEntityBuffers();
		// entity storage
		vector<Entity> entities;

};
