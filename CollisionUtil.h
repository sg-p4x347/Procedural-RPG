#pragma once
#include "pch.h"
using DirectX::SimpleMath::Vector3;
namespace CollisionUtil {
	struct GjkIntersection {
		GjkIntersection() {}
		GjkIntersection( std::vector<Vector3> & simplex, Vector3 direction) : Simplex(simplex), Direction(direction) {

		}
		std::vector<Vector3> Simplex;
		Vector3 Direction;
	};
	Vector3 & HullSupport(std::vector<Vector3> & hull, Vector3 & d) {
		Vector3 & max = hull[0];
		float maxDot = -INFINITY;
		for (auto & vertex : hull) {
			auto dot = vertex.Dot(d);
			if (dot > maxDot) {
				max = vertex;
				maxDot = dot;
			}
		}
		return max;
	}
	void NearestSimplexHelper(GjkIntersection & result, Vector3 & a, Vector3 & b, Vector3 & ao, Vector3 & ab) {
		if (ab.Dot(ao) > 0) {
			result.Simplex = std::vector<Vector3>{ a,b };
			result.Direction = ab.Cross(ao).Cross(ab);
		}
		else {
			result.Simplex = std::vector<Vector3>{ a };
			result.Direction = ao;
		}
	}
	bool NearestSimplex(GjkIntersection & result) {
		Vector3 ao = -result.Simplex[result.Simplex.size() - 1];
		if (result.Simplex.size() == 2) {
			Vector3 & a = result.Simplex[1];
			Vector3 & b = result.Simplex[0];
			Vector3 ab = b - a;

			result.Direction = ab.Cross(ao).Cross(ab);

		}
		else if (result.Simplex.size() == 3) {
			Vector3 & a = result.Simplex[2];
			Vector3 & b = result.Simplex[0];
			Vector3 & c = result.Simplex[1];

			Vector3 ab = b - a;
			Vector3 ac = c - a;
			Vector3 abc = ab.Cross(ac);

			Vector3 acNorm = abc.Cross(ac);
			Vector3 abNorm = ab.Cross(abc);

			if (acNorm.Dot(ao) > 0) {
				result.Simplex = std::vector<Vector3>{ a,c };
				result.Direction = ac.Cross(ao).Cross(ac);
			} else if (abNorm.Dot(ao) > 0) {
				result.Simplex = std::vector<Vector3>{ a,b };
				result.Direction = ab.Cross(ao).Cross(ab);
			} else {
				result.Simplex = std::vector<Vector3>{ a,b,c };
				result.Direction = abc;
			}
		}
		else if (result.Simplex.size() == 4) {
			// points
			Vector3 & a = result.Simplex[3];
			Vector3 & b = result.Simplex[0];
			Vector3 & c = result.Simplex[1];
			Vector3 & d = result.Simplex[2];
			// edges
			Vector3 ab = b - a;
			Vector3 ac = c - a;
			Vector3 ad = d - a;
			// faces
			Vector3 abc = ab.Cross(ac);
			Vector3 adb = ad.Cross(ab);
			Vector3 acd = ac.Cross(ad);

			if (abc.Dot(ao) > 0) {
				if (adb.Dot(ao) > 0) {
					// abc & adb
					result.Simplex = std::vector<Vector3>{ a,b };
					result.Direction = ab.Cross(ao).Cross(ab);
				}
				else if (acd.Dot(ao) > 0) {
					// abc & acd
					result.Simplex = std::vector<Vector3>{ a,c };
					result.Direction = ac.Cross(ao).Cross(ac);
				}
				else {
					// abc
					result.Simplex = std::vector<Vector3>{ a,b,c };
					result.Direction = abc;
				}
			}
			else if (adb.Dot(ao) > 0) {
				if (acd.Dot(ao) > 0) {
					// adb & acd
					result.Simplex = std::vector<Vector3>{ a,d };
					result.Direction = ad.Cross(ao).Cross(ad);
				}
				else {
					// adb
					result.Simplex = std::vector<Vector3>{ a,d,b };
					result.Direction = adb;
				}
			}
			else if (acd.Dot(ao) > 0) {
				// acd
				result.Simplex = std::vector<Vector3>{ a,c,d };
				result.Direction = acd;
			}
			else {
				// origin enclosed, yay!
				return true;
			}
		}
		return false;
	}
	
	bool GJK( std::vector<Vector3> & hullA, std::vector<Vector3> & hullB, GjkIntersection & result) {
		Vector3 initialAxis = Vector3::Up;
		Vector3 a = HullSupport(hullA, initialAxis) - HullSupport(hullB, -initialAxis);
		result.Simplex = std::vector<Vector3>{ a };
		result.Direction = -a;
		while (true) {
			a = HullSupport(hullA, result.Direction) - HullSupport(hullB, -result.Direction);
			if (a.Dot(result.Direction) < 0) {
				return false;
			}
			result.Simplex.push_back(a);
			if (NearestSimplex(result)) {
				return true;
			}
		}

	}
	
}