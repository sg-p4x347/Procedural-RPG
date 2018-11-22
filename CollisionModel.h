#pragma once
#include "ConvexHull.h"
namespace geometry {
	struct CollisionModel
	{
		CollisionModel();
		std::vector<ConvexHull> hulls;
	};
}
