#include "pch.h"
#include "MeshPart.h"

namespace geometry {
	MeshPart::MeshPart() : alpha(false)
	{
	}
	MeshPart::~MeshPart()
	{
	}
	void MeshPart::SetMaterial(shared_ptr<Material> mat)
	{
		materialName = mat->name;
		material = mat;
		alpha |= mat->alpha != 1.f;
	}
	void MeshPart::Import(std::istream & ifs)
	{
		DeSerialize(materialName,ifs);
		size_t vertexCount = 0;
		DeSerialize(vertexCount, ifs);
		vertices.resize(vertexCount);
		for (size_t vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {
			VertexPositionNormalTangentColorTexture vertex;
			DeSerialize(vertex, ifs);
			vertices[vertexIndex] = vertex;
		}
		size_t indexCount = 0;
		DeSerialize(indexCount, ifs);
		indices.resize(indexCount);
		for (size_t indexIndex = 0; indexIndex < indexCount; indexIndex++) {
			uint16_t index = 0;
			DeSerialize(index, ifs);
			indices[indexIndex] = index;
		}
		
	}
	void MeshPart::Export(std::ostream & ofs)
	{
		Serialize(materialName, ofs);
		Serialize(vertices.size(), ofs);
		if (vertices.size() > 0)
			ofs.write((const char *)&vertices[0], vertices.size() * sizeof(VertexPositionNormalTangentColorTexture));
		Serialize(indices.size(), ofs);
		if (indices.size() > 0)
			ofs.write((const char *)&indices[0], indices.size() * sizeof(uint16_t));
	}
}