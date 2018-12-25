#include "pch.h"
#include "Cylinder.h"

namespace geometry {
	Cylinder::Cylinder()
	{
	}

	Cylinder::Cylinder(float radius, Vector3 axis, Vector3 center) : 
		radius(radius), 
		axis(axis),
		center(center)
	{
	}


	Cylinder::~Cylinder()
	{
	}

	float Cylinder::Length()
	{
		return axis.Length();
	}

	Vector3 geometry::Cylinder::Support(Vector3 direction)
	{
		Vector3 radial = axis.Cross(direction).Cross(axis);
		radial.Normalize();
		if (axis.Dot(direction) > 0.f) {
			return center + axis * 0.5f + radial * radius;
		}
		else {
			return center - axis * 0.5f + radial * radius;
		}
	}

	shared_ptr<CollisionVolume> geometry::Cylinder::Transform(DirectX::SimpleMath::Matrix matrix)
	{
		return shared_ptr<CollisionVolume>(new Cylinder(
			radius,
			axis,
			Vector3::Transform(center,matrix)
		));
	}
	BoundingBox Cylinder::Bounds()
	{
		if (axis.x == 0 && axis.z == 0) {
			return BoundingBox(center, XMFLOAT3(radius, Length(), radius));
		}
	}
	std::vector<Vector3> Cylinder::Normals()
	{
		auto result = std::vector<Vector3>{axis};
		result[0].Normalize();
		return result;
	}
}
