#pragma once
#include "VBO.h"

class VboParser
{
public:
	VboParser(Filesystem::path & directory);
	template <typename VertexType>
	inline void ExportVBO(Filesystem::path path, shared_ptr<Components::VBO<VertexType>> vbo) {
		std::ofstream vboFile(m_directory / path, std::ofstream::out | std::ifstream::binary);
		if (!vboFile.is_open())
			throw std::exception("VboParser : " + m_directory / path + " not found");

		vboFile << (uint32_t)vbo->Vertices.size();
		vboFile << (uint32_t)vbo->Indices.size();

		vboFile.write(reinterpret_cast<char*>(&vbo->Vertices.front()), sizeof(VertexType) * vbo->Vertices.size());
		vboFile.write(reinterpret_cast<char*>(&vbo->Indices.front()), sizeof(uint16_t) * vbo->Indices.size());
	}
	template <typename VertexType>
	inline void ImportVBO(Filesystem::path path, shared_ptr<Components::VBO<VertexType>> vbo) {
		std::ifstream vboFile(m_directory / path, std::ifstream::in | std::ifstream::binary);
		if (!vboFile.is_open())
			throw std::exception("VboParser : " + m_directory / path + " not found");

		uint32_t numVertices = 0;
		uint32_t numIndices = 0;

		vboFile.read(reinterpret_cast<char*>(&numVertices), sizeof(uint32_t));
		if (!numVertices)
			throw std::exception("VboParser : vertex count was zero");

		vboFile.read(reinterpret_cast<char*>(&numIndices), sizeof(uint32_t));
		if (!numIndices)
			throw std::exception("VboParser : index count was zero");

		vbo->Vertices.resize(numVertices);
		vboFile.read(reinterpret_cast<char*>(&vbo->Vertices.front()), sizeof(VertexType) * numVertices);

		vbo->Indices.resize(numIndices);
		vboFile.read(reinterpret_cast<char*>(&vbo->Indices.front()), sizeof(uint16_t) * numIndices);
	}
private:
	Filesystem::path m_directory;
};

