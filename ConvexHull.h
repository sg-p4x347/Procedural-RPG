#pragma once
#include "CollisionVolume.h"
namespace geometry {
	class ConvexHull : public CollisionVolume
	{
	public:
		ConvexHull();
		ConvexHull(DirectX::BoundingBox && box);
		void AddVertex(Vector3 vertex);
		void AddAxis(Vector3 normal);
		virtual std::pair<Vector3, Vector3> BiSupport(Vector3 direction) override;
		bool WithinTolerance(float a, float b);
		// Inherited via CollisionVolume
		virtual Vector3 Support(Vector3 direction) override;
	public:
		std::vector<Vector3> vertices;
		std::vector<Vector3> axes;
		float radius;
		static const float axisTolerance;

		

		// Inherited via CollisionVolume
		virtual shared_ptr<CollisionVolume> Transform(DirectX::SimpleMath::Matrix matrix) override;



		// Inherited via CollisionVolume
		virtual std::vector<Vector3> Normals() override;


		// Inherited via ISerialization
		virtual void Import(std::istream & ifs) override;

		virtual void Export(std::ostream & ofs) override;

	};
}
