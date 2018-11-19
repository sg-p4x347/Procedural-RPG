#pragma once
#include "MeshPart.h"
namespace geometry {
	class Mesh
	{
	public:
		Mesh();
		~Mesh();
		void AddPart(MeshPart & part);
		vector<MeshPart> & GetParts();
	private:
		vector<MeshPart> m_parts;
	};
}
