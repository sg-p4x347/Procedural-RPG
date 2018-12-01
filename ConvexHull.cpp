#include "pch.h"
#include "ConvexHull.h"

namespace geometry {
	const float ConvexHull::axisTolerance = 0.01f;
	ConvexHull::ConvexHull()
	{
	}

	ConvexHull::ConvexHull(DirectX::BoundingBox && box)
	{
		XMFLOAT3 corners[8];
		box.GetCorners(&corners[0]);
		for (int i = 0; i < 8; i++) {
			AddVertex(corners[i]);
		}
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
	Vector3 & ConvexHull::Max(Vector3 direction)
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
	std::pair<Vector3, Vector3> ConvexHull::MinMax(Vector3 direction)
	{
		Vector3 min = vertices[0];
		Vector3 max = vertices[0];
		float minDot = INFINITY;
		float maxDot = -INFINITY;
		for (auto & vertex : vertices) {
			auto dot = vertex.Dot(direction);
			if (dot > maxDot) {
				max = vertex;
				maxDot = dot;
			}
			if (dot < minDot) {
				min = vertex;
				minDot = dot;
			}
		}
		return std::make_pair(min,max);
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
	DirectX::BoundingBox ConvexHull::Bounds()
	{
		auto xMinMax = MinMax(Vector3::UnitX);
		auto yMinMax = MinMax(Vector3::UnitY);
		auto zMinMax = MinMax(Vector3::UnitZ);
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
}