#pragma once
#include "CollisionVolume.h"
namespace geometry {
	struct CollisionModel
	{
		CollisionModel();
		std::vector<shared_ptr<CollisionVolume>> volumes;
		void Merge(CollisionModel & other);
		CollisionModel Transform(Matrix matrix);
	};
}
