#pragma once
#include "pch.h"
namespace geometry {
	Vector2 Clamp(Rectangle rect, Vector2 point);
	struct Int3 {
		Int3(int pX, int pY, int pZ);
		int x;
		int y;
		int z;
		Int3 operator-(Int3 & other);
		Int3 operator+(Int3 & other);
	};
	float ClosestPointOnLine(Vector3 linePoint, Vector3 lineDir, Vector3 point);
	void ClosestPoints(Vector3 p, Vector3 pDir, Vector3 q, Vector3 qDir, float & pt, float & qt);
	bool LinePlaneIntersection(Vector3 linePoint, Vector3 lineDir, Vector3 planePoint, Vector3 planeNormal, float & t);
	bool RayIntersectCylinder(Vector3 rayStart, Vector3 rayDirection, Vector3 cylinderStart, Vector3 cylinderAxis, float cylinderRadius);
	bool RayIntersectTriangle(Vector3 rayStart, Vector3 rayDirection, Vector3 a, Vector3 b, Vector3 c);
	bool RayIntersectSphere(Vector3 rayStart, Vector3 rayDirection, Vector3 sphereCenter, float sphereRadius);
	bool RayIntersectHalfSphere(Vector3 rayStart, Vector3 rayDirection, Vector3 sphereCenter, float sphereRadius, Vector3 sphereNormal);
}