#pragma once
#include "EntityTypedefs.h"
#include "Material.h"
#include "ISerialization.h"
namespace geometry {
	struct MeshPart
		: public ISerialization
	{
		MeshPart();
		~MeshPart();
		bool alpha;
		vector<VertexPositionNormalTangentColorTexture> vertices;
		vector<uint16_t> indices;
		string materialName;
		shared_ptr<Material> material;
		void SetMaterial(shared_ptr<Material> mat);
		
		// Inherited via ISerialization
		virtual void Import(std::istream & ifs) override;
		virtual void Export(std::ostream & ofs) override;
	};
}
