#include "pch.h"
#include "Region.h"
#include "Utility.h"

using namespace DirectX::SimpleMath;
using namespace Utility;

Region::Region() {
}
void Region::init(ID3D11Device * device, int x, int z, unsigned int worldWidthIn, unsigned int regionWidthIn, string workingPathIn) {
	worldWidth = worldWidthIn;
	regionWidth = regionWidthIn;
	workingPath = workingPathIn;
	m_device = device;
	// fill the buffers with data from world files
	if (x >= 0 && z >= 0 && x < worldWidth / regionWidth && z < worldWidth / regionWidth) {
		regionX = x;
		regionZ = z;
		loadTerrain();
		loadObjects();
		loadEntities();
		Null = false;
	}
	else {
		Null = true;
	}
}
HRESULT Region::loadTerrain() {
	HRESULT result = S_OK;

	unsigned int regionIndex = posToIndex(regionX, regionZ, worldWidth / regionWidth);
	unsigned int vertexCount = (regionWidth + 1)*(regionWidth + 1);
	unsigned int regionSize = vertexCount * sizeof(short);

	// Load the vertex array with data.
	float * heightMap = new float[vertexCount];
	Vector3 * normalMap = new Vector3[vertexCount];
	ifstream terrain(workingPath + "terrain.bin", ios::binary);
	ifstream normal(workingPath + "normal.bin", ios::binary);
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
		for (int i = 0; i < vertexCount; i++) {
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
	//// create 2 triangles (6 vertex indices) for every quad in the region
	//unsigned int * faces = new unsigned int[regionWidth*regionWidth * 6];
	//unsigned int index = 0;
	//for (int z = 0; z < regionWidth; z++) {
	//	for (int x = 0; x < regionWidth; x++) {
	//		unsigned int vertex1 = posToIndex(x, z, regionWidth + 1);
	//		unsigned int vertex2 = posToIndex(x + 1, z, regionWidth + 1);
	//		unsigned int vertex3 = posToIndex(x, z + 1, regionWidth + 1);
	//		unsigned int vertex4 = posToIndex(x + 1, z + 1, regionWidth + 1);
	//		if (pythag(1, heightMap[vertex1]-heightMap[vertex4], 1) < pythag(1, heightMap[vertex2] - heightMap[vertex3], 1)) {
	//			/*
	//			1---2
	//			| \ |
	//			3---4
	//			*/
	//			faces[index] = vertex1;
	//			index++;
	//			faces[index] = vertex2;
	//			index++;
	//			faces[index] = vertex4;
	//			index++;

	//			faces[index] = vertex4;
	//			index++;
	//			faces[index] = vertex3;
	//			index++;
	//			faces[index] = vertex1;
	//			index++;
	//		} else {
	//			/*
	//			1---2
	//			| / |
	//			3---4
	//			*/
	//			faces[index] = vertex3;
	//			index++;
	//			faces[index] = vertex1;
	//			index++;
	//			faces[index] = vertex2;
	//			index++;

	//			faces[index] = vertex2;
	//			index++;
	//			faces[index] = vertex4;
	//			index++;
	//			faces[index] = vertex3;
	//			index++;
	//		}
	//	}
	//}
	//// Create a normal vector for each vertex
	//Vector3 * normals = new Vector3[vertexCount];
	//
	//for (int z = 0; z < regionWidth+1; z++) {
	//	for (int x = 0; x < regionWidth + 1; x++) {
	//		Vector3 sum(0.f, 0.f, 0.f);
	//		// loop through the 4 quads around this vertex
	//		for (int quadZ = z; quadZ >= 0 && quadZ <= regionWidth; quadZ--) {
	//			for (int quadX = x;  quadX >= 0 && quadX <= regionWidth; quadX--) {
	//				// check both faces
	//				for (int faceIndex = posToIndex(quadX, quadZ, regionWidth) * 6; faceIndex < posToIndex(quadX, quadZ, regionWidth) * 6 + 6; faceIndex += 3) {
	//					// check to see if this face includes the vertex
	//					if (faces[faceIndex] == posToIndex(x, z, regionWidth + 1) || faces[faceIndex + 1] == posToIndex(x, z, regionWidth + 1) || faces[faceIndex + 2] == posToIndex(x, z, regionWidth + 1)) {
	//						Vector3 vertex1 = Vector3(indexToX(faceIndex, regionWidth + 1), heightMap[faces[faceIndex]], indexToY(faces[faceIndex], regionWidth + 1));
	//						Vector3 vertex2 = Vector3(indexToX(faceIndex + 1, regionWidth + 1), heightMap[faces[faceIndex + 1]], indexToY(faces[faceIndex + 1], regionWidth + 1));
	//						Vector3 vertex3 = Vector3(indexToX(faceIndex + 2, regionWidth + 1), heightMap[faces[faceIndex + 2]], indexToY(faces[faceIndex + 2], regionWidth + 1));
	//						sum += (vertex2 - vertex1).Cross(vertex2 - vertex3);
	//					}
	//				}
	//			}
	//		}
	//		// average the weighted normals by normalizing the vector
	//		sum.Normalize();
	//		normals[posToIndex(x, z, regionWidth + 1)] = sum;
	//	}
	//}
	// create 2 triangles (6 vertices) for every quad in the region
	m_terrainVertices = new VertexPositionNormalTangentColorTexture[regionWidth * regionWidth * 6];
	m_terrainIndices = new unsigned int[regionWidth * regionWidth * 6];

	unsigned int index = 0;
	for (int z = 0; z < regionWidth; z++) {
		for (int x = 0; x < regionWidth; x++) {
			// Get the indexes to the four points of the quad.
			Vector3 vertex1 = Vector3(x + (regionX * regionWidth), heightMap[((regionWidth + 1) * z) + x], z + (regionZ * regionWidth));          // Upper left.
			Vector3 vertex2 = Vector3(x + 1 + (regionX*regionWidth), heightMap[((regionWidth + 1) * z) + (x + 1)], z + (regionZ * regionWidth));      // Upper right.
			Vector3 vertex3 = Vector3(x + (regionX*regionWidth), heightMap[((regionWidth + 1) * (z + 1)) + x], z + 1 + (regionZ * regionWidth));      // Bottom left.
			Vector3 vertex4 = Vector3(x + 1 + (regionX*regionWidth), heightMap[((regionWidth + 1) * (z + 1)) + (x + 1)], z + 1 + (regionZ * regionWidth));  // Bottom right.


			// determine which two vertices are closest and make that an edge
			//if (pythag(1, vertex1.y - vertex4.y, 1) < pythag(1, vertex2.y - vertex3.y, 1)) {
				/*
				1---2
				| \ |
				3---4
				*/
				// Now create two triangles for that quad.
				// Triangle 1 - Upper left.
			m_terrainVertices[index] = {
				XMFLOAT3(vertex1),			// position
				XMFLOAT3(normalMap[posToIndex(x,z,regionWidth + 1)]),		// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),	// tangent
				XMFLOAT4(1.f,255.f,1.f,1.f),					// color
				XMFLOAT2(0.f,0.f)			// texture
			};
			m_terrainIndices[index] = index;
			index++;
			// Triangle 1 - Bottom right.
			m_terrainVertices[index] = {
				XMFLOAT3(vertex4),			// position
				XMFLOAT3(normalMap[posToIndex(x + 1,z + 1,regionWidth + 1)]),		// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),	// tangent
				XMFLOAT4(1.f,255.f,1.f,1.f),					// color
				XMFLOAT2(1.f,1.f)			// texture
			};
			m_terrainIndices[index] = index;
			index++;
			// Triangle 1 - Bottom left.
			m_terrainVertices[index] = {
				XMFLOAT3(vertex3),			// position
				XMFLOAT3(normalMap[posToIndex(x,z + 1,regionWidth + 1)]),		// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),	// tangent
				XMFLOAT4(1.f,255.f,1.f,1.f),					// color
				XMFLOAT2(0.f,1.f)			// texture
			};
			m_terrainIndices[index] = index;
			index++;
			// Triangle 2 - Upper left.
			m_terrainVertices[index] = {
				XMFLOAT3(vertex1),			// position
				XMFLOAT3(normalMap[posToIndex(x,z,regionWidth + 1)]),		// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),	// tangent
				XMFLOAT4(1.f,255.f,1.f,1.f),					// color
				XMFLOAT2(0.f,0.f)			// texture
			};
			m_terrainIndices[index] = index;
			index++;
			// Triangle 2 - Upper right.
			m_terrainVertices[index] = {
				XMFLOAT3(vertex2),			// position
				XMFLOAT3(normalMap[posToIndex(x + 1,z,regionWidth + 1)]),		// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),	// tangent
				XMFLOAT4(1.f,255.f,1.f,1.f),					// color
				XMFLOAT2(1.f,0.f)			// texture
			};
			m_terrainIndices[index] = index;
			index++;
			// Triangle 2 - Bottom right.
			m_terrainVertices[index] = {
				XMFLOAT3(vertex4),			// position
				XMFLOAT3(normalMap[posToIndex(x + 1,z + 1,regionWidth + 1)]),		// normal
				XMFLOAT4(1.f,0.f,0.f,1.f),	// tangent
				XMFLOAT4(1.f,255.f,1.f,1.f),					// color
				XMFLOAT2(1.f,1.f)			// texture
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
		desc.ByteWidth = sizeof(VertexPositionNormalTangentColorTexture)*regionWidth*regionWidth * 6;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA initData = { 0 };
		initData.pSysMem = m_terrainVertices;

		DX::ThrowIfFailed(m_device->CreateBuffer(&desc, &initData,
			terrainVB.ReleaseAndGetAddressOf()));
	}
	// Create the Index Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = sizeof(unsigned int)*regionWidth*regionWidth * 6;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA initData = { 0 };
		initData.pSysMem = m_terrainIndices;

		DX::ThrowIfFailed(m_device->CreateBuffer(&desc, &initData,
			terrainIB.ReleaseAndGetAddressOf()));
	}
	return S_OK;
}
void Region::render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, PrimitiveBatch<DirectX::VertexPositionColor> * batch) {

	// Set vertex buffer stride and offset.
	UINT stride = sizeof(VertexPositionNormalTangentColorTexture);
	UINT offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, terrainVB.GetAddressOf(), &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(terrainIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw
	deviceContext->DrawIndexed(regionWidth * regionWidth * 6, 0, 0);
	return;
}
void Region::loadObjects() {

}
void Region::loadEntities() {

}
void Region::updateEntityBuffers() {

}
// getters
Microsoft::WRL::ComPtr<ID3D11Buffer> Region::getTerrainVB() {
	return terrainVB;
};
Microsoft::WRL::ComPtr<ID3D11Buffer> Region::getTerrainIB() {
	return terrainIB;
};
int Region::getIndexCount() {
	return regionWidth * regionWidth * 6;
}
bool Region::isNull() {
	return Null;
}
Region::~Region() {
	delete[] m_terrainVertices;
	delete[] m_terrainIndices;
}
