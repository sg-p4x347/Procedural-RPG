#include "pch.h"
#include "Geometry.h"
namespace geometry {
	Vector2 Clamp(Rectangle rect, Vector2 point)
	{
		return Vector2(Utility::Clamp((long)point.x, rect.x, rect.x + rect.width), Utility::Clamp((long)point.y, rect.y, rect.y + rect.height));
	}
	float ClosestPointOnLine(Vector3 linePoint, Vector3 lineDir, Vector3 point)
	{
		return (point.Dot(lineDir) - 2 * linePoint.Dot(lineDir)) / lineDir.LengthSquared();
	}
	void ClosestPoints(Vector3 p, Vector3 pDir, Vector3 q, Vector3 qDir, float & pt, float & qt)
	{
		Vector3 w = p - q;
		float a = p.LengthSquared();
		float b = pDir.Dot(qDir);
		float c = q.LengthSquared();
		float d = pDir.Dot(w);
		float e = qDir.Dot(w);
		pt = (b * e - c * d) / (a * c - b * b);
		qt = (a * e - b * d) / (a * c - b * b);
	}
	bool LinePlaneIntersection(Vector3 linePoint, Vector3 lineDir, Vector3 planePoint, Vector3 planeNormal, float & t)
	{
		float nDotDir = planeNormal.Dot(lineDir);
		if (nDotDir != 0.f) {
			t = (planeNormal.Dot(planePoint) - planeNormal.Dot(linePoint)) / nDotDir;
			return true;
		}
		// parallel
		return false;
	}
	bool RayIntersectCylinder(Vector3 rayStart, Vector3 rayDirection, Vector3 cylinderStart, Vector3 cylinderAxis, float cylinderRadius)
	{
		float rt, ct;
		ClosestPoints(rayStart, rayDirection, cylinderStart, cylinderAxis, rt, ct);
		if (rt >= 0.f && ct >= 0.f && ct <= 1.f) {
			Vector3 rayPoint = rayStart + rt * rayDirection;
			Vector3 cylinderPoint = cylinderStart + ct * cylinderAxis;
			return Vector3::DistanceSquared(rayPoint, cylinderPoint) <= cylinderRadius * cylinderRadius;
		}
		return false;
	}
	bool RayIntersectTriangle(Vector3 rayStart, Vector3 rayDirection, Vector3 a, Vector3 b, Vector3 c)
	{
		float t;
		Vector3 ba = b - a;
		Vector3 cb = c - b;
		Vector3 ac = a - c;
		Vector3 normal = ba.Cross(ac);
		if (LinePlaneIntersection(rayStart, rayDirection, a, normal, t)) {
			if (t >= 0.f) {
				Vector3 q = rayStart + t * rayDirection;
				Vector3 qa = q - a;
				Vector3 qb = q - b;
				Vector3 qc = q - c;
				return ba.Cross(qa).Dot(normal) >= 0
					&& cb.Cross(qb).Dot(normal) >= 0
					&& ac.Cross(qc).Dot(normal) >= 0;
			}
			// Behind the ray
			return false;
		}
		// Parallel
		return false;
	}
	bool RayIntersectSphere(Vector3 rayStart, Vector3 rayDirection, Vector3 sphereCenter, float sphereRadius)
	{
		float t = ClosestPointOnLine(rayStart, rayDirection, sphereCenter);
		return t >= 0.f && ((rayStart + t * rayDirection) - sphereCenter).LengthSquared() <= sphereRadius * sphereRadius;
	}
	bool RayIntersectHalfSphere(Vector3 rayStart, Vector3 rayDirection, Vector3 sphereCenter, float sphereRadius, Vector3 sphereNormal)
	{
		float t = ClosestPointOnLine(rayStart, rayDirection, sphereCenter);
		Vector3 closestPoint = rayStart + t * rayDirection;
		return t >= 0.f 
			&& (closestPoint - sphereCenter).Dot(sphereNormal) >= 0.f
			&& (closestPoint - sphereCenter).LengthSquared() <= sphereRadius * sphereRadius;
	}
	Int3::Int3(int pX, int pY, int pZ) : x(pX), y(pY), z(pZ)
	{
	}
	Int3 Int3::operator-(Int3 & other)
	{
		return Int3(x - other.x, y - other.y, z - other.z);
	}
	Int3 Int3::operator+(Int3 & other)
	{
		return Int3(x + other.x, y + other.y, z + other.z);
	}
}
