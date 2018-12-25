#include "pch.h"
#include "CollisionVolume.h"

namespace geometry {
	CollisionVolume::CollisionVolume()
	{
	}


	CollisionVolume::~CollisionVolume()
	{
	}
	std::pair<Vector3, Vector3> CollisionVolume::BiSupport(Vector3 direction)
	{
		return std::pair<Vector3, Vector3>(Support(-direction),Support(direction));
	}
	BoundingBox CollisionVolume::Bounds()
	{
		auto xMinMax = BiSupport(Vector3::UnitX);
		auto yMinMax = BiSupport(Vector3::UnitY);
		auto zMinMax = BiSupport(Vector3::UnitZ);
		return BoundingBox(
			Vector3(
			(xMinMax.first.x + xMinMax.second.x) * 0.5f,
				(yMinMax.first.y + yMinMax.second.y) * 0.5f,
				(zMinMax.first.z + zMinMax.second.z) * 0.5f
			),
			Vector3(
			(xMinMax.second.x - xMinMax.first.x),
				(yMinMax.second.y - yMinMax.first.y),
				(zMinMax.second.z - zMinMax.first.z)
			)
		);
	}
	std::pair<float, float> CollisionVolume::Project(Vector3 direction)
	{
		auto points = BiSupport(direction);
		return std::pair<float, float>(points.first.Dot(direction),points.second.Dot(direction));
	}
}