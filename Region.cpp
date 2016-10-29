#include "pch.h"
#include "Region.h"
#include "Utility.h"

using namespace DirectX::SimpleMath;
using namespace Utility;

Region::Region() {
}
void Region::Initialize(ID3D11Device * device, int x, int z, unsigned int worldWidth, unsigned int regionWidth, string name) {
	m_worldWidth = worldWidth;
	m_regionWidth = regionWidth;
	// fill the buffers with data from world files
	if (x >= 0 && z >= 0 && x < signed(m_worldWidth / m_regionWidth) && z < signed(m_worldWidth / m_regionWidth)) {
		m_regionX = x;
		m_regionZ = z;
		LoadTerrain(device,name);
		LoadObjects();
		LoadEntities();
		m_null = false;
	}
	else {
		m_null = true;
	}
}
void Region::LoadTerrain(ID3D11Device * device, string name) {
	unsigned int regionIndex = posToIndex(m_regionX, m_regionZ, m_worldWidth / m_regionWidth);
	unsigned int vertexCount = (m_regionWidth + 1)*(m_regionWidth + 1);
	unsigned int regionSize = vertexCount * sizeof(short);
	string workingPath = "saves/" + name + "/";

	// Load the vertex array with data.
	float * heightMap = new float[vertexCount];
	Vector3 * normalMap = new Vector3[vertexCount];
	ifstream terrain(workingPath + "/terrain.bin", ios::binary);
	ifstream normal(workingPath + "/normal.bin", ios::binary);
	if (terrain.is_open() && normal.is_open()) {
		// stores the exact bytes from the file into memory
		char *terrainCharBuffer = new char[regionSize];
		char *normalCharBuffer = new char[vertexCount * 3];
		// move start position to the region, and proceed to read
		terrain.seekg(regionSize*regionIndex);
		terrain.read((char *)terrainCharBuffer, regionSize);

		normal.seekg(vertexCount * 3 * regionIndex);
		normal.read((char *)normalCharBuffer, vertexCount * 3);

		// Fill up the heightmap and normalmap from the file blob
		for (unsigned int i = 0; i < vertexCount; i++) {
			// heightMap
			unsigned short upper = terrainCharBuffer[(i * 2) + 1] << 8;
			unsigned char lower = terrainCharBuffer[i * 2];
			float vertex = short(upper | lower) / 10.f;
			heightMap[i] = vertex;
			// normalMap
			float x = float(normalCharBuffer[i * 3]);
			float y = float(normalCharBuffer[i * 3 + 1]);
			float z = float(normalCharBuffer[i * 3 + 2]);
			normalMap[i] = Vector3(x, y, z);
			normalMap[i].Normalize();
		}
		terrain.close();
		normal.close();
		delete[] terrainCharBuffer;
		delete[] normalCharBuffer;
	}
	// create 2 triangles (6 vertices) for every quad in the region
	m_terrainVertices = new VertexPositionNormalTangentColorTexture[m_regionWidth * m_regionWidth * 6];
	m_terrainIndices = new unsigned int[m_regionWidth * m_regionWidth * 6];

	unsigned int index = 0;
	for (unsigned int z = 0; z < m_regionWidth; z++) {
		for (unsigned int x = 0; x < m_regionWidth; x++) {
			// Get the indexes to the four points of the quad.
			Vector3 vertex1 = Vector3(x + (m_regionX * m_regionWidth), heightMap[((m_regionWidth + 1) * z) + x], z + (m_regionZ * m_regionWidth));          // Upper left.
			Vector3 vertex2 = Vector3(x + 1 + (m_regionX*m_regionWidth), heightMap[((m_regionWidth + 1) * z) + (x + 1)], z + (m_regionZ * m_regionWidth));      // Upper right.
			Vector3 vertex3 = Vector3(x + (m_regionX*m_regionWidth), heightMap[((m_regionWidth + 1) * (z + 1)) + x], z + 1 + (m_regionZ * m_regionWidth));      // Bottom left.
			Vector3 vertex4 = Vector3(x + 1 + (m_regionX*m_regionWidth), heightMap[((m_regionWidth + 1) * (z + 1)) + (x + 1)], z + 1 + (m_regionZ * m_regionWidth));  // Bottom right.

			/*
			1---2
			| \ |
			3---4
			*/

			// Triangle 1 - Upper left.
			m_terrainVertices[index] = {
				XMFLOAT3(vertex1),										// position
				XMFLOAT3(normalMap[posToIndex(x,z,m_regionWidth + 1)]),	// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),								// tangent
				XMFLOAT4(1.f,255.f,1.f,1.f),							// color
				XMFLOAT2(0.f,0.f)										// texture
			};
			m_terrainIndices[index] = index;
			index++;
			// Triangle 1 - Bottom right.
			m_terrainVertices[index] = {
				XMFLOAT3(vertex4),												// position
				XMFLOAT3(normalMap[posToIndex(x + 1,z + 1,m_regionWidth + 1)]),	// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),										// tangent
				XMFLOAT4(1.f,255.f,1.f,1.f),									// color
				XMFLOAT2(1.f,1.f)												// texture
			};
			m_terrainIndices[index] = index;
			index++;
			// Triangle 1 - Bottom left.
			m_terrainVertices[index] = {
				XMFLOAT3(vertex3),											// position
				XMFLOAT3(normalMap[posToIndex(x,z + 1,m_regionWidth + 1)]),	// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),									// tangent
				XMFLOAT4(1.f,255.f,1.f,1.f),								// color
				XMFLOAT2(0.f,1.f)											// texture
			};
			m_terrainIndices[index] = index;
			index++;
			// Triangle 2 - Upper left.
			m_terrainVertices[index] = {
				XMFLOAT3(vertex1),										// position
				XMFLOAT3(normalMap[posToIndex(x,z,m_regionWidth + 1)]),	// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),								// tangent
				XMFLOAT4(1.f,255.f,1.f,1.f),							// color
				XMFLOAT2(0.f,0.f)										// texture
			};
			m_terrainIndices[index] = index;
			index++;
			// Triangle 2 - Upper right.
			m_terrainVertices[index] = {
				XMFLOAT3(vertex2),											// position
				XMFLOAT3(normalMap[posToIndex(x + 1,z,m_regionWidth + 1)]),	// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),									// tangent
				XMFLOAT4(1.f,255.f,1.f,1.f),								// color
				XMFLOAT2(1.f,0.f)											// texture
			};
			m_terrainIndices[index] = index;
			index++;
			// Triangle 2 - Bottom right.
			m_terrainVertices[index] = {
				XMFLOAT3(vertex4),												// position
				XMFLOAT3(normalMap[posToIndex(x + 1,z + 1,m_regionWidth + 1)]),	// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),										// tangent
				XMFLOAT4(1.f,255.f,1.f,1.f),									// color
				XMFLOAT2(1.f,1.f)												// texture
			};
			m_terrainIndices[index] = index;
			index++;
		}
	}
	delete[] heightMap;
	delete[] normalMap;
	// Create the Vertex Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = sizeof(VertexPositionNormalTangentColorTexture)*m_regionWidth*m_regionWidth * 6;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA initData = { 0 };
		initData.pSysMem = m_terrainVertices;

		DX::ThrowIfFailed(device->CreateBuffer(&desc, &initData,
			m_terrainVB.ReleaseAndGetAddressOf()));
	}
	// Create the Index Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = sizeof(unsigned int)*m_regionWidth*m_regionWidth * 6;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA initData = { 0 };
		initData.pSysMem = m_terrainIndices;

		DX::ThrowIfFailed(device->CreateBuffer(&desc, &initData,
			m_terrainIB.ReleaseAndGetAddressOf()));
	}
}
void Region::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, PrimitiveBatch<DirectX::VertexPositionColor> * batch) {

	// Set vertex buffer stride and offset.
	UINT stride = sizeof(VertexPositionNormalTangentColorTexture);
	UINT offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, m_terrainVB.GetAddressOf(), &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_terrainIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw
	deviceContext->DrawIndexed(m_regionWidth * m_regionWidth * 6, 0, 0);
	return;
}
void Region::LoadObjects() {

}
void Region::LoadEntities() {

}
void Region::UpdateEntityBuffers() {

}
// getters
Microsoft::WRL::ComPtr<ID3D11Buffer> Region::GetTerrainVB() {
	return m_terrainVB;
};
Microsoft::WRL::ComPtr<ID3D11Buffer> Region::GetTerrainIB() {
	return m_terrainIB;
};
int Region::GetIndexCount() {
	return m_regionWidth * m_regionWidth * 6;
}
bool Region::IsNull() {
	return m_null;
}
Region::~Region() {
	delete[] m_terrainVertices;
	delete[] m_terrainIndices;
}
