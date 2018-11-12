#pragma once
#include "MeshPart.h"
namespace geometry {
	class Mesh
	{
	public:
		Mesh();
		~Mesh();
	private:
		vector<MeshPart> m_parts;
	};
}
