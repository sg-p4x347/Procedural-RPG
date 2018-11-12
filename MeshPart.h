#pragma once
#include "EntityTypedefs.h"
namespace geometry {
	class MeshPart
	{
	public:
		MeshPart();
		~MeshPart();
	private:
		bool m_alpha;
		vector<VertexPositionNormalTangentColorTexture> m_vertices;
		vector<uint16_t> m_indices;
	};
}
