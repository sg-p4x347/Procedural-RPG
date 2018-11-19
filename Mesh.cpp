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
}