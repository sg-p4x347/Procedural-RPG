#pragma once
namespace geometry {
	struct ConvexHull
	{
		
		ConvexHull();
		ConvexHull(DirectX::BoundingBox && box);
		void AddVertex(Vector3 && vertex);
		void AddAxis(Vector3 & normal);
		Vector3 & Max(Vector3 direction);
		std::pair<Vector3, Vector3> MinMax(Vector3 direction);
		bool WithinTolerance(float a, float b);
		ConvexHull Transform(Matrix matrix);
		DirectX::BoundingBox Bounds();
		std::vector<Vector3> vertices;
		std::vector<Vector3> axes;
		float radius;
		static const float axisTolerance;
	};
}
