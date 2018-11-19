#pragma once
#include "EntityTypedefs.h"
#include "Material.h"
namespace geometry {
	struct MeshPart
	{
		MeshPart();
		bool alpha;
		vector<VertexPositionNormalTangentColorTexture> vertices;
		vector<uint16_t> indices;
		shared_ptr<Material> material;
	};
}
