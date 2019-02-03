#include "pch.h"
#include "Mesh.h"

namespace geometry {

	Mesh::Mesh()
	{
	}


	Mesh::~Mesh()
	{
	}
	void Mesh::AddPart(MeshPart & part)
	{
		m_parts.push_back(part);
	}
	vector<MeshPart>& Mesh::GetParts()
	{
		return m_parts;
	}
	void Mesh::Import(std::istream & ifs)
	{
		size_t partCount = 0;
		DeSerialize(partCount, ifs);
		for (size_t partIndex = 0; partIndex < partCount; partIndex++) {
			MeshPart part;
			part.Import(ifs);
			m_parts.push_back(part);
		}
	}
	void Mesh::Export(std::ostream & ofs)
	{
		Serialize(m_parts.size(),ofs);
		for (auto & part : m_parts)
			part.Export(ofs);
	}
}