#include "pch.h"
#include "Region.h"
#include "Utility.h"

using namespace DirectX::SimpleMath;
using namespace Utility;

Region::Region(const string & directory) : m_directory(directory) {
}
void Region::Initialize(ID3D11Device * device, int x, int z, unsigned int worldWidth, unsigned int regionWidth, string name, vector<shared_ptr<Architecture::Building>> & buildings) {
	// delete the old arrays
	if (m_terrainVertices && !m_null) delete[] m_terrainVertices;
	if (m_terrainIndices && !m_null) delete[] m_terrainIndices;
	// set widths
	m_worldWidth = worldWidth;
	m_regionWidth = regionWidth;
	// fill the buffers with data from world files
	if (x >= 0 && z >= 0 && x < (m_worldWidth / m_regionWidth) && z < (m_worldWidth / m_regionWidth)) {
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
	// set the buildings
	m_buildings = m_buildings;
}
void Region::LoadTerrain(ID3D11Device * device, string name) {
	unsigned int regionIndex = posToIndex(m_regionX, m_regionZ, m_worldWidth / m_regionWidth);
	unsigned int vertexCount = (m_regionWidth + 1)*(m_regionWidth + 1);
	unsigned int rowSize = (m_regionWidth + 1) * sizeof(short);
	unsigned int regionSize = vertexCount * sizeof(short);
	string workingPath = "saves/" + name + "/";

	// Load the vertex array with data.
	vector<float> heightMap;
	vector<Vector3> normalMap;
	ifstream terrainStream(workingPath + "/terrain.bin", ios::binary);
	ifstream normalStream(workingPath + "/normal.bin", ios::binary);
	if (terrainStream.is_open() && normalStream.is_open()) {
		// stores the exact bytes from the file into memory
		char *terrainCharBuffer = new char[regionSize];
		char *normalCharBuffer = new char[vertexCount * 3];
		// move start position to the region, and proceed to read each line into the Char buffers
		for (int vertZ = 0; vertZ <= m_regionWidth; vertZ++) {
			for (int vertX = 0; vertX <= m_regionWidth; vertX++) {
				int index = posToIndex(vertX + m_regionX * m_regionWidth, vertZ + m_regionZ * m_regionWidth, m_worldWidth + 1);
				// heightMap
				char shortBuffer[2];
				terrainStream.seekg(index * sizeof(short));
				terrainStream.read((char *)shortBuffer, sizeof(short));
				unsigned char lower = (unsigned char) shortBuffer[0];
				unsigned short upper = (unsigned short) shortBuffer[1] << 8;
				
				float vertex = short(upper | lower) / 10.f;
				heightMap.push_back(vertex);
				// normalMap
				char normalBuffer[3];
				normalStream.seekg(index * 3 * sizeof(char));
				normalStream.read((char *)normalBuffer, 3 * sizeof(char));
				Vector3 normal = Vector3(float(normalBuffer[0]), float(normalBuffer[1]), float(normalBuffer[2]));
				normal.Normalize();
				normalMap.push_back(normal);
			}
		}

		terrainStream.close();
		normalStream.close();
		delete[] terrainCharBuffer;
		delete[] normalCharBuffer;
	}
	// create 2 triangles (6 vertices) for every quad in the region
	m_terrainVertices = new VertexPositionNormalTangentColorTexture[m_regionWidth * m_regionWidth * 6];
	m_terrainIndices = new unsigned int[m_regionWidth * m_regionWidth * 6];

	int index = 0;
	for (int z = 0; z < m_regionWidth; z++) {
		for (int x = 0; x < m_regionWidth; x++) {
			// Get the indexes to the four points of the quad.
			Vector3 vertex1 = Vector3(float(x + (m_regionX * m_regionWidth)), heightMap[((m_regionWidth + 1) * z) + x], float(z + (m_regionZ * m_regionWidth)));          // Upper left.
			Vector3 vertex2 = Vector3(float(x + 1 + (m_regionX*m_regionWidth)), heightMap[((m_regionWidth + 1) * z) + (x + 1)], float(z + (m_regionZ * m_regionWidth)));      // Upper right.
			Vector3 vertex3 = Vector3(float(x + (m_regionX*m_regionWidth)), heightMap[((m_regionWidth + 1) * (z + 1)) + x], float(z + 1 + (m_regionZ * m_regionWidth)));      // Bottom left.
			Vector3 vertex4 = Vector3(float(x + 1 + (m_regionX*m_regionWidth)), heightMap[((m_regionWidth + 1) * (z + 1)) + (x + 1)], float(z + 1 + (m_regionZ * m_regionWidth)));  // Bottom right.

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
void Region::RenderModels(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext)
{
	// Render buildings
	for (std::shared_ptr<Architecture::Building> building : m_buildings) {
		building->Render(deviceContext);
	}
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
Rectangle Region::GetArea()
{
	return Rectangle(m_regionX * m_regionWidth, m_regionZ * m_regionWidth,m_regionWidth,m_regionWidth);
}
string Region::GetDirectory()
{
	return m_directory + "/" + std::to_string(m_regionX) + ',' + std::to_string(m_regionZ);
}
Region::~Region() {
	delete[] m_terrainVertices;
	delete[] m_terrainIndices;
}
