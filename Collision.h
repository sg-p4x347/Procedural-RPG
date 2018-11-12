#pragma once
#include "WorldComponent.h"
#include "ISerialization.h"
#include "AssetTypes.h"
namespace world {
	class Collision :
		public WorldComponent,
		public ISerialization
	{
	public:
		Collision();
		Collision(EntityID asset, AssetType type);
		// Persisted Data
		EntityID Asset;
		AssetType Type;
		bool Enabled;
		int Colliding;

		// Runtime Data
		std::vector<Vector3> CollisionNormals;

		// Inherited via ISerialization
		virtual void Import(std::istream & ifs) override;
		virtual void Export(std::ostream & ofs) override;
		// Inherited via Component
		//virtual void Import(std::istream & ifs) override;
		//virtual void Export(std::ostream & ofs) override;
	};
}

