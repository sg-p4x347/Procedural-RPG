#include "pch.h"
#include "CollisionModel.h"

namespace geometry {
	CollisionModel::CollisionModel()
	{
	}
	void CollisionModel::Merge(CollisionModel & other)
	{
		volumes.insert(volumes.end(),other.volumes.begin(), other.volumes.end());
	}
	CollisionModel CollisionModel::Transform(Matrix matrix)
	{
		CollisionModel prime;
		for (auto & volume : volumes) {
			prime.volumes.push_back(volume->Transform(matrix));
		}
		return prime;
	}
}