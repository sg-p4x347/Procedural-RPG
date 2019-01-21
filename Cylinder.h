#pragma once
#include "CollisionVolume.h"
namespace geometry {
	class Cylinder
		: public CollisionVolume
	{
	public:
		Cylinder();
		Cylinder(float radius, Vector3 axis, Vector3 center);
		~Cylinder();
	public:
		float radius;
		Vector3 axis;
		Vector3 center;

		float Length();
		// Inherited via CollisionVolume
		virtual Vector3 Support(Vector3 direction) override;
		virtual shared_ptr<CollisionVolume> Transform(DirectX::SimpleMath::Matrix matrix) override;

		// Inherited via CollisionVolume
		virtual BoundingBox Bounds() override;

		// Inherited via CollisionVolume
		virtual std::vector<Vector3> Normals() override;

		// Inherited via CollisionVolume
		virtual void Import(std::istream & ifs) override;
		virtual void Export(std::ostream & ofs) override;
	};
}
