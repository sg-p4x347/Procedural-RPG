#pragma once
#include "WorldComponent.h"
#include "ISerialization.h"
#include "AssetTypes.h"

namespace world {
	struct ContactInfo {
		ContactInfo() : contactNormal(Vector3::Zero) {}
		Vector3 contactNormal;
	};
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
		std::vector<ContactInfo> Contacts;

		// Inherited via ISerialization
		virtual void Import(std::istream & ifs) override;
		virtual void Export(std::ostream & ofs) override;
		// Inherited via Component
		//virtual void Import(std::istream & ifs) override;
		//virtual void Export(std::ostream & ofs) override;
	};
}

