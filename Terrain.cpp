#include "pch.h"
#include "Terrain.h"

namespace Component {
	
	Terrain::Terrain()
	{
	}

	Terrain::Terrain(const unsigned int & id)
	{

	}
	

	Terrain::~Terrain()
	{
	}

	shared_ptr<Component> Terrain::GetComponent(const unsigned int & id)
	{
		// Query in-memory list
		for (Terrain & terrain : GetComponents()) {
			if (terrain.ID == id) {
				return std::make_shared<Component>(&terrain);
			}
		}
		// Not found
		return shared_ptr<Component>(nullptr);
	}

	void Terrain::SaveAll(string directory)
	{
		for (Terrain & terrain : GetComponents()) {
			terrain.Save(directory);
		}
	}

	void Terrain::Attach(shared_ptr<Component> component)
	{
		GetComponents().push_back(*dynamic_cast<Terrain*>(component.get()));
	}

	string Terrain::GetName()
	{
		return "Terrain";
	}

	shared_ptr<Component> Terrain::Create(std::ifstream & ifs)
	{
		return shared_ptr<Component>(new Terrain(ifs));
	}

	Terrain::Terrain(std::ifstream & ifs)
	{
		Import(ifs);
	}

	vector<Terrain>& Terrain::GetComponents()
	{
		static vector<Terrain> components;
		return components;
	}
	void Terrain::Import(std::ifstream & ifs)
	{
		//Component::Import(ifs);
		//DeSerialize(TerrainMap.width, ifs);

	}
	void Terrain::Export(std::ofstream & ofs)
	{
		//Component::Export(ofs);
		//Serialize(TerrainMap.width, ofs);
		//Serialize(BiomeMap.width, ofs);
		////-------------------------------
		//// save terrain to binary file
		////-------------------------------

		//// Terrain Map
		//unsigned int vertexCount = (TerrainMap.width + 1) * (TerrainMap.width + 1);
		//unique_ptr<unsigned char[]> terrainBuffer(new unsigned char[vertexCount * sizeof(short)]);

		//// Normal Map
		//unique_ptr<char[]> normalBuffer(new char[vertexCount * 3]); // X (8 bits) Y (8 bits) Z (8 bits)

		//															// Biome Map
		//unsigned int sampleSpacing = TerrainMap.width / BiomeMap.width;
		//unsigned int sampleCount = (BiomeMap.width + 1) * (BiomeMap.width + 1);
		//unique_ptr<unsigned char[]> biomeBuffer(new unsigned char[sampleCount * sizeof(float)]);



		//unsigned int index = 0;
		//unsigned int biomeIndex = 0;

		//for (unsigned short vertZ = 0; vertZ <= TerrainMap.width; vertZ++) {
		//	for (unsigned short vertX = 0; vertX <= TerrainMap.width; vertX++) {
		//		// verticies
		//		float vertex = TerrainMap.map[vertX][vertZ];
		//		terrainBuffer.get()[index * sizeof(short)] = short(vertex * 10) & 0xff;
		//		terrainBuffer.get()[index * sizeof(short) + 1] = (short(vertex * 10) >> 8) & 0xff;
		//		// biome
		//		if (vertZ % sampleSpacing == 0 && vertX % sampleSpacing == 0) {
		//			float floatValue = BiomeMap.map[vertX / sampleSpacing][vertZ / sampleSpacing];
		//			memcpy(&biomeBuffer.get()[biomeIndex * sizeof(float)], &floatValue, sizeof(float));
		//			/*biomeFile.seekp(biomeIndex * sizeof(float));
		//			biomeFile.write((char *)charValue, sizeof(float));*/
		//			biomeIndex++;
		//		}
		//		// normals
		//		float left = signed(vertX) - 1 >= 0 ? TerrainMap.map[vertX - 1][vertZ] : vertex;
		//		float right = signed(vertX) + 1 <= signed(TerrainMap.width) ? TerrainMap.map[vertX + 1][vertZ] : vertex;
		//		float up = signed(vertZ) + 1 <= signed(TerrainMap.width) ? TerrainMap.map[vertX][vertZ + 1] : vertex;
		//		float down = signed(vertZ) - 1 >= 0 ? TerrainMap.map[vertX][vertZ - 1] : vertex;

		//		DirectX::SimpleMath::Vector3 normal = DirectX::SimpleMath::Vector3(left - right, 2.f, down - up);
		//		normal.Normalize();
		//		NormalMap.Map[vertX][vertZ] = normal;

		//		// scale the vector up into the 8-bit range
		//		normal *= 128;
		//		normalBuffer.get()[index * 3] = char(normal.x);
		//		normalBuffer.get()[index * 3 + 1] = char(normal.y);
		//		normalBuffer.get()[index * 3 + 2] = char(normal.z);

		//		// update vertex index
		//		index++;
		//	}
		//}

		////-----------------------------------------------
		//// output file stream
		////-----------------------------------------------

		//// TerrainMap
		//ofs.write((const char *)terrainBuffer.get(), vertexCount * sizeof(short));

		//// BiomeMap
		//ofs.write((const char *)biomeBuffer.get(), sampleCount * sizeof(float));

		//// NormalMap
		//ofs.write((const char *)normalBuffer.get(), vertexCount * 3);
	}
}