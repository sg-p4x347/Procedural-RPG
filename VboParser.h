#pragma once
#include "VBO.h"

class VboParser
{
public:
	VboParser(Filesystem::path & directory);
	template <typename VertexType>
	inline void ExportVBO(Filesystem::path path, Components::VBO<VertexType> & vbo) {
		path = m_directory / (path.string() + ".vbo");
		std::ofstream vboFile(path, std::ofstream::binary);
		if (!vboFile.is_open())
			throw std::exception(("VboParser : " + path.string() + " not found").c_str());

		uint32_t numVertices = vbo.Vertices.size();
		uint32_t numIndices = vbo.Indices.size();

		vboFile.write((char*)(&numVertices),sizeof(uint32_t));
		vboFile.write((char*)(&numIndices), sizeof(uint32_t));

		vboFile.write(reinterpret_cast<char*>(&vbo.Vertices.front()), sizeof(VertexType) * vbo.Vertices.size());
		vboFile.write(reinterpret_cast<char*>(&vbo.Indices.front()), sizeof(uint16_t) * vbo.Indices.size());
	}
	template <typename VertexType>
	inline void ImportVBO(Filesystem::path path, Components::VBO<VertexType> & vbo) {
		path = m_directory / (path.string() + ".vbo");
		std::ifstream vboFile(path, std::ifstream::binary);
		if (!vboFile.is_open())
			throw std::exception(("VboParser : " + path.string() + " not found").c_str());

		uint32_t numVertices = 0;
		uint32_t numIndices = 0;

		vboFile.read(reinterpret_cast<char*>(&numVertices), sizeof(uint32_t));
		if (!numVertices || numVertices > 80000)
			throw std::exception("VboParser : vertex count was out of bounds");

		vboFile.read(reinterpret_cast<char*>(&numIndices), sizeof(uint32_t));
		if (!numIndices || numVertices > 80000)
			throw std::exception("VboParser : index count was out of bounds");

		vbo.Vertices.resize(numVertices);
		vboFile.read(reinterpret_cast<char*>(&vbo.Vertices.front()), sizeof(VertexType) * numVertices);

		vbo.Indices.resize(numIndices);
		vboFile.read(reinterpret_cast<char*>(&vbo.Indices.front()), sizeof(uint16_t) * numIndices);
	}
private:
	Filesystem::path m_directory;
};

