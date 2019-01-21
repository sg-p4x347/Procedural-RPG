#include "pch.h"
#include "CollisionModel.h"
#include "Cylinder.h"
#include "ConvexHull.h"

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
	void CollisionModel::Import(std::istream & ifs)
	{
		size_t volumeCount = 0;
		DeSerialize(volumeCount, ifs);
		for (int volumeIndex = 0; volumeIndex < volumeCount; volumeIndex++) {
			string typeName;
			DeSerialize(typeName, ifs);
			shared_ptr<CollisionVolume> volume;
			if (typeName == "Cylinder") {
				volume = shared_ptr<CollisionVolume>(new Cylinder());
			} else if (typeName == "ConvexHull") {
				volume = shared_ptr<CollisionVolume>(new ConvexHull());
			}
			volume->Import(ifs);
			volumes.push_back(volume);
		}
	}
	void CollisionModel::Export(std::ostream & ofs)
	{
		Serialize(volumes.size(), ofs);
		for (auto & volume : volumes) {
			volume->Export(ofs);
		}
	}
}