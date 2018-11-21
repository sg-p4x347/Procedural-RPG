#include "pch.h"
#include "ConvexHull.h"

namespace geometry {
	const float ConvexHull::axisTolerance = 0.01f;
	ConvexHull::ConvexHull()
	{
	}

	void ConvexHull::AddVertex(Vector3 && vertex)
	{
		auto length = vertex.Length();
		radius = std::max(length, radius);
		vertices.push_back(vertex);
	}
	void ConvexHull::AddAxis(Vector3 & normal)
	{
		normal.Normalize();
		// check for equivalent (within small tolerance) normals
		for (auto & n : axes) {
			if ((n - normal).Length() <= axisTolerance || (n + normal).Length() <= axisTolerance)
				return;
		}
		// normal not found, add it
		axes.push_back(normal);
	}
	Vector3 & ConvexHull::Support(Vector3 direction)
	{
		Vector3 & max = vertices[0];
		float maxDot = -INFINITY;
		for (auto & vertex : vertices) {
			auto dot = vertex.Dot(direction);
			if (dot > maxDot) {
				max = vertex;
				maxDot = dot;
			}
		}
		return max;
	}
	bool ConvexHull::WithinTolerance(float a, float b)
	{
		return a - axisTolerance <= b && a + axisTolerance >= b;
	}
	ConvexHull ConvexHull::Transform(Matrix matrix)
	{
		ConvexHull result;
		for (auto & vertex : vertices) {
			result.vertices.push_back(Vector3::Transform(vertex,matrix));
		}
		result.radius = radius;
		auto Oprime = Vector3::Transform(Vector3::Zero, matrix);
		for (auto & axis : axes) {
			result.axes.push_back(Vector3::Transform(axis, matrix) - Oprime);
		}
		return result;
	}
}