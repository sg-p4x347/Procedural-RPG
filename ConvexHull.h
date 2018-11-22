#pragma once
namespace geometry {
	struct ConvexHull
	{
		
		ConvexHull();
		void AddVertex(Vector3 && vertex);
		void AddAxis(Vector3 & normal);
		Vector3 & Support(Vector3 direction);
		bool WithinTolerance(float a, float b);
		ConvexHull Transform(Matrix matrix);
		std::vector<Vector3> vertices;
		std::vector<Vector3> axes;
		float radius;
		static const float axisTolerance;
	};
}
