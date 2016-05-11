#include "Region.h"

Region::Region() {
}
void Region::init(ID3D11Device* device, int x, int z, unsigned int worldWidthIn, unsigned int regionWidthIn, string workingPathIn) {
	worldWidth = worldWidthIn;
	regionWidth = regionWidthIn;
	workingPath = workingPathIn;
	// fill the buffers with data from world files
	if (x >= 0 && z >= 0 && x < worldWidth/regionWidth && z < worldWidth/regionWidth) {
		regionX = x;
		regionZ = z;
		loadTerrain(device);
		loadObjects();
		loadEntities();
		Null = false;
	} else {
		Null = true;
	}
}
HRESULT Region::loadTerrain(ID3D11Device * device) {
	HRESULT result = S_OK;

	unsigned int regionIndex = posToIndex(regionX, regionZ, worldWidth / regionWidth);
	unsigned int regionSize = (regionWidth + 1)*(regionWidth + 1)*sizeof(short);

	// temporarily store vertices and indices
	vector<SimpleVertex> terrainVertices;
	vector<unsigned int> terrainIndices;

	// Load the vertex array with data.
	ifstream file(workingPath + "terrain.bin", ios::binary);
	if (file.is_open()) {
		// stores the exact bytes from the file into memory
		unsigned char *terrainCharBuffer = new unsigned char[regionSize];
		// stores the signed short values for each vertex
		short *terrain = new short[regionSize / 2];
		
		// move start position to the region, and proceed to read
		file.seekg(regionSize*regionIndex);
		file.read((char *)terrainCharBuffer, regionSize);
		for (int i = 0; i < regionSize; i += 2) {
			float vertex = float(terrainCharBuffer[i + 1] << 8 | terrainCharBuffer[i]) / 10;
			// push this vertex to the vertexBuffer
			float range = 32;
			float intensity = (vertex + range / 2) / range;
			float r = intensity, g = intensity, b = intensity;
			if (vertex > 0) {
				if (vertex <= 0.5) {
					// beach
					b *= 0.6;
				}
				else if (vertex <= 4) {
					// grass
					r = 0;
					g *= 0.5;
					b = 0;
				}
				else if (vertex <= 7.5) {
					// dirt
					r *= 0.6;
					g *= 0.5;
					b *= 0.4;
				}
			}
			// find the x and z coordinates of this vertex
			int z = floor((i / 2) / (regionWidth+1));
			int x = (i / 2) - (z * (regionWidth + 1));
			terrainVertices.push_back({ XMFLOAT3(x + regionWidth*regionX, vertex, z + regionWidth*regionZ), XMFLOAT4(r,g,b,1.0) });
		}
		delete[] terrainCharBuffer;
		// push triangle indices to the indexBuffer
		for (int index = 0; index < regionSize / 2; index++) {
			// find the x and z coordinates of this vertex
			int z = floor(index / (regionWidth + 1));
			int x = index - (z * (regionWidth + 1));
			if (x < regionWidth && z < regionWidth) {
				// push indices
				if (pythag(1, terrainVertices[index].Pos.y - terrainVertices[index + (regionWidth + 1) + 1].Pos.y, 1) < pythag(1, terrainVertices[index + (regionWidth + 1)].Pos.y - terrainVertices[index + 1].Pos.y, 1)) {
					// bottom triangle
					terrainIndices.push_back(unsigned int(index));
					terrainIndices.push_back(unsigned int(index + (regionWidth + 1) + 1));
					terrainIndices.push_back(unsigned int(index + 1));
					// top triangle						
					terrainIndices.push_back(unsigned int(index));
					terrainIndices.push_back(unsigned int(index + (regionWidth + 1)));
					terrainIndices.push_back(unsigned int(index + (regionWidth + 1) + 1));
				}
				else {
					// bottom triangle
					terrainIndices.push_back(unsigned int(index));
					terrainIndices.push_back(unsigned int(index + (regionWidth + 1)));
					terrainIndices.push_back(unsigned int(index + 1));
					// top triangle						
					terrainIndices.push_back(unsigned int(index + (regionWidth + 1)));
					terrainIndices.push_back(unsigned int(index + (regionWidth + 1) + 1));
					terrainIndices.push_back(unsigned int(index + 1));
				}
				// create texture coordinates

			}
		}
		vertices = terrainVertices;
		delete[] terrain;
		indexCount = terrainIndices.size();
	} else {
		return E_FAIL;
	}
	file.close();
	

	// push vertices to static vertex and index buffer members
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SimpleVertex) * terrainVertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = terrainVertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &terrainVB);
	if (FAILED(result)) {
		return result;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = terrainIndices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &terrainIB);
	if (FAILED(result))
	{
		return false;
	}
}
void Region::render(ID3D11DeviceContext* deviceContext) {
	// put the vertex and index buffers on the graphics piepeline
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(SimpleVertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &terrainVB, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(terrainIB, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}
void Region::loadObjects() {

}
void Region::loadEntities() {

}
void Region::updateEntityBuffers() {

}
// getters
int Region::getIndexCount() {
	return indexCount;
}
bool Region::isNull() {
	return Null;
}
Region::~Region() {
	delete terrainVB;
	delete terrainIB;
}
