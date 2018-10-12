#pragma once
#include "pch.h"
using DirectX::SimpleMath::Vector3;
namespace CollisionUtil {
	struct GjkIntersection {
		GjkIntersection(const std::vector<Vector3> & simplex, Vector3 direction) : Simplex(simplex), Direction(direction) {

		}
		std::vector<Vector3> Simplex;
		Vector3 Direction;
	};
	bool GJK(const std::vector<Vector3> & a, const std::vector<Vector3> & b, GjkIntersection & result) {
		Vector3 initialAxis = Vector3::Up;
		Vector3 A = HullSupport(a, initialAxis) - HullSupport(b, initialAxis * -1);
		result.Simplex = std::vector<Vector3>{ A };
		result.Direction = A * -1;

	}
	Vector3 HullSupport(const std::vector<Vector3> & hull, Vector3 & d) {
		Vector3 max;
		double maxDot = -INFINITY;
		for (auto & vertex : hull) {
			auto dot = vertex.Dot(d);
			if (dot > maxDot) {
				max = vertex;
			}
		}
		return max;
	}
	bool NearestSimplex(GjkIntersection & result) {
		Vector3 ao = result.Simplex[result.Simplex.size() - 1] * -1;
		if (result.Simplex.size() == 2) {
			Vector3 & a = result.Simplex[1];
			Vector3 & b = result.Simplex[0];
			Vector3 ab = b - a;
			if (ab.Dot(ao) > 0) {
				result.Direction = ab.Cross(ao).Cross(ab);
				return false;
			}
			else {
				result.Simplex = std::vector<Vector3>{ a };
				result.Direction = ao;
			}

		}
		else if (result.Simplex.size() == 3) {
			Vector3 & a = result.Simplex[2];
			Vector3 & b = result.Simplex[1];
			Vector3 & c = result.Simplex[0];

			Vector3 ab = b - a;
			Vector3 ac = c - a;

			Vector3 acDir = ac.Cross(ac.Cross(ab));
			

			Vector3 
		}
	}
}