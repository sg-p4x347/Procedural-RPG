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

	void ConvexHull::AddVertex(Vector3 vertex)
	{
		auto length = vertex.Length();
		radius = std::max(length, radius);
		vertices.push_back(vertex);
	}
	void ConvexHull::AddAxis(Vector3 normal)
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
	std::pair<Vector3, Vector3> ConvexHull::BiSupport(Vector3 direction)
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
	Vector3 ConvexHull::Support(Vector3 direction)
	{
		Vector3 max = vertices[0];
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
	shared_ptr<CollisionVolume> ConvexHull::Transform(DirectX::SimpleMath::Matrix matrix)
	{
		shared_ptr<ConvexHull> result(new ConvexHull());
		for (auto & vertex : vertices) {
			result->vertices.push_back(Vector3::Transform(vertex, matrix));
		}
		result->radius = radius;
		auto Oprime = Vector3::Transform(Vector3::Zero, matrix);
		for (auto & axis : axes) {
			result->axes.push_back(Vector3::Transform(axis, matrix) - Oprime);
		}
		return static_pointer_cast<CollisionVolume>(result);
	}
	std::vector<Vector3> ConvexHull::Normals()
	{
		return axes;
	}
	void ConvexHull::Import(std::istream & ifs)
	{
		size_t vertexCount = 0;
		DeSerialize(vertexCount, ifs);
		vertices.resize(vertexCount);
		for (size_t vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {
			Vector3 vertex;
			DeSerialize(vertex, ifs);
			AddVertex(vertex);
		}
		size_t axisCount = 0;
		DeSerialize(axisCount, ifs);
		axes.resize(axisCount);
		for (size_t axisIndex = 0; axisIndex < vertexCount; axisIndex++) {
			Vector3 axis;
			DeSerialize(axis, ifs);
			AddAxis(axis);
		}

	}
	void ConvexHull::Export(std::ostream & ofs)
	{
		string type = "ConvexHull";
		Serialize(type, ofs);
		Serialize(vertices.size(), ofs);
		if (vertices.size() > 0)
			ofs.write((const char *)&vertices[0], vertices.size() * sizeof(Vector3));
		Serialize(axes.size(), ofs);
		if (axes.size() > 0)
			ofs.write((const char *)&axes[0], axes.size() * sizeof(Vector3));
	}
}