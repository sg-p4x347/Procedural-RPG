#pragma once
#include "CollisionVolume.h"
#include "ISerialization.h"
namespace geometry {
	struct CollisionModel :
		public ISerialization
	{
		CollisionModel();
		std::vector<shared_ptr<CollisionVolume>> volumes;
		void Merge(CollisionModel & other);
		CollisionModel Transform(Matrix matrix);

		// Inherited via ISerialization
		virtual void Import(std::istream & ifs) override;
		virtual void Export(std::ostream & ofs) override;
	};
}
