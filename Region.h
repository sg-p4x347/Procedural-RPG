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
		void Initialize(ID3D11Device * device, int x, int z, unsigned int worldWidth, unsigned int regionWidth, string name);
		// put buffers on graphics pipeline
		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, PrimitiveBatch<DirectX::VertexPositionColor> * batch);
		// getters
		Microsoft::WRL::ComPtr<ID3D11Buffer> GetTerrainVB();
		Microsoft::WRL::ComPtr<ID3D11Buffer> GetTerrainIB();
		int GetIndexCount();
		bool IsNull();
	private:
		// nullness
		bool m_null = true;

		// vertex buffers
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_terrainVB;
		//vector<VertexPositionNormalTangentColorTexture> terrainVertices;
		VertexPositionNormalTangentColorTexture * m_terrainVertices = nullptr;

		// index buffers
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_terrainIB;
		//vector<unsigned int> terrainIndices;
		unsigned int * m_terrainIndices = nullptr;

		// properties
		int m_regionX;
		int m_regionZ;
		int m_worldWidth;
		int m_regionWidth;

		// load terrain into terrain buffers
		void LoadTerrain(ID3D11Device * device, string name);
		// load objects into object buffers
		void LoadObjects();
		// load entities into the entity storage vector
		void LoadEntities();
		// update entity vertices
		void UpdateEntityBuffers();
		// entity storage
		vector<Entity> m_entities;

};

