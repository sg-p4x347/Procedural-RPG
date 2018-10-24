#pragma once
#include "pch.h"
using DirectX::SimpleMath::Vector3;
namespace CollisionUtil {
	struct Simplex {
		Simplex() : Size(0), A(Vertices[0]), B(Vertices[1]), C(Vertices[2]), D(Vertices[3]) {}
		Vector3 Vertices[4];
		unsigned int Size;
		Vector3 & A;
		Vector3 & B;
		Vector3 & C;
		Vector3 & D;
		void Push(Vector3 vertex) {
			for (unsigned int i = Size; i > 0; i--) {
				Vertices[i] = Vertices[i - 1];
			}
			Vertices[0] = vertex;
			Size++;
		}
		void Set(Vector3 a) { A = a; Size = 1; }
		void Set(Vector3 a, Vector3 b) { A = a; B = b; Size = 2; }
		void Set(Vector3 a, Vector3 b, Vector3 c) { A = a; B = b; C = c; Size = 3;  }
	};
	struct GjkIntersection {
		GjkIntersection() {}
		Simplex Simplex;
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
			result.Simplex.Set(a,b);
			result.Direction = ab.Cross(ao).Cross(ab);
		}
		else {
			result.Simplex.Set(a);
			result.Direction = ao;
		}
	}
	bool NearestSimplex(GjkIntersection & result) {
		if (result.Simplex.Size == 2) {
			Vector3 & a = result.Simplex.A;
			Vector3 & b = result.Simplex.B;
			Vector3 ab = b - a;
			result.Direction = ab.Cross(-a).Cross(ab);
			
		}
		else if (result.Simplex.Size == 3) {
			Vector3 & a = result.Simplex.A;
			Vector3 & b = result.Simplex.B;
			Vector3 & c = result.Simplex.C;
			Vector3 ao = -a;
			Vector3 ab = b - a;
			Vector3 ac = c - a;
			Vector3 abc = ab.Cross(ac);

			if (abc.Cross(ac).Dot(ao) > 0) {
				result.Simplex.Set(c,a);
				result.Direction = ac.Cross(ao).Cross(ac);
			} else if (ab.Cross(abc).Dot(ao) > 0) {
				result.Simplex.Set(b,a);
				result.Direction = ab.Cross(ao).Cross(ab);
			}
			else {
				float v = abc.Dot(ao);
				
				if(v > 0.f) {
					result.Direction = abc;
				}
				else if (v < 0.f) {
					result.Simplex.Set(c,a, b);
					result.Direction = -abc;
				}
				else {
					return true;
				}
			}
		}
		else  {

			{
				Vector3 & a = result.Simplex.A;
				Vector3 & b = result.Simplex.B;
				Vector3 & c = result.Simplex.C;
				Vector3 & d = result.Simplex.D;
				Vector3 ao = -a;
				Vector3 ab = b - a;
				Vector3 ac = c - a;
				if (ab.Cross(ac).Dot(ao) > 0) {
					goto jmp_face;
				}

				const Vector3 ad = d - a;

				if (ac.Cross(ad).Dot(ao) > 0) {
					result.Simplex.Set(a, c, d);
					goto jmp_face;
				}

				if (ad.Cross(ab).Dot(ao) > 0) {
					// origin is in front of triangle adb, simplex is set to this triangle
					// go to jmp_face
					result.Simplex.Set(a, d, b);
					goto jmp_face;
				}
				return true;
			}
		jmp_face:
			Vector3 & a = result.Simplex.A;
			Vector3 & b = result.Simplex.B;
			Vector3 & c = result.Simplex.C;
			Vector3 ao = -a;
			const Vector3 ab = b - a;
			const Vector3 ac = c - a;
			const Vector3 abc = ab.Cross(ac);

			if (ab.Cross(abc).Dot(ao) > 0) {
				result.Simplex.Set(b,a);
				result.Direction = ab.Cross(ao).Cross(ab);
				return false;
			}

			if (abc.Cross(ac).Dot(ao) > 0) {
				result.Simplex.Set(c,a);
				result.Direction = ac.Cross(ao).Cross(ac);
				return false;
			}
			result.Simplex.Set(c,a,b);
			result.Direction = abc;
			return false;
			//if (abc.Dot(ao) > 0) {
			//	if (adb.Dot(ao) > 0) {
			//		// abc & adb
			//		result.Simplex = std::vector<Vector3>{ a,b };
			//		result.Direction = ab.Cross(ao).Cross(ab);
			//	}
			//	else if (acd.Dot(ao) > 0) {
			//		// abc & acd
			//		result.Simplex = std::vector<Vector3>{ a,c };
			//		result.Direction = ac.Cross(ao).Cross(ac);
			//	}
			//	else {
			//		// abc
			//		result.Simplex = std::vector<Vector3>{ a,b,c };
			//		result.Direction = abc;
			//	}
			//}
			//else if (adb.Dot(ao) > 0) {
			//	if (acd.Dot(ao) > 0) {
			//		// adb & acd
			//		result.Simplex = std::vector<Vector3>{ a,d };
			//		result.Direction = ad.Cross(ao).Cross(ad);
			//	}
			//	else {
			//		// adb
			//		result.Simplex = std::vector<Vector3>{ a,d,b };
			//		result.Direction = adb;
			//	}
			//}
			//else if (acd.Dot(ao) > 0) {
			//	// acd
			//	result.Simplex = std::vector<Vector3>{ a,c,d };
			//	result.Direction = acd;
			//}
			//else {
			//	// origin enclosed, yay!
			//	return true;
			//}
		}
		return false;
	}
	
	bool GJK( std::vector<Vector3> & hullA, std::vector<Vector3> & hullB, GjkIntersection & result) {
		Vector3 initialAxis = Vector3::Up;
		Vector3 a = HullSupport(hullA, initialAxis) - HullSupport(hullB, -initialAxis);
		result.Simplex.Set(a);
		result.Direction = -a;
		
		for (int i = 0; i < 20;i++) {
			a = HullSupport(hullA, result.Direction) - HullSupport(hullB, -result.Direction);
			if (a.Dot(result.Direction) < 0) {
				return false;
			}
			result.Simplex.Push(a);
			if (NearestSimplex(result)) {
				return true;
			}
		}
		return false;
	}
	struct SatResult {
		SatResult() {}
		Vector3 Axis;

	};
	struct SatProjection {
		SatProjection() : SatProjection(INFINITY,-INFINITY) {}
		SatProjection(float min, float max) : Min(min), Max(max) {

		}
		bool Overlaps(SatProjection & projection) {
			return projection.Max > Min && Max > projection.Min;
		}
		float Min;
		float Max;
	};
	
	SatProjection Project(Vector3 & axis, std::vector<Vector3> & hull) {
		SatProjection projection;
		for (auto & vertex : hull) {
			float dot = vertex.Dot(axis);
			if (dot < projection.Min) {
				projection.Min = dot;
			}
			if (dot > projection.Max) {
				projection.Max = dot;
			}
		}
		return projection;
	}
	// generate axes from boxes
	std::vector<Vector3> GenerateSatAxes(SimpleMath::Matrix transformA, SimpleMath::Matrix transformB) {
		std::vector<Vector3> axes;
		axes.push_back(Vector3::Transform(Vector3::Up, transformA));
		axes.push_back(Vector3::Transform(Vector3::Forward, transformA));
		axes.push_back(Vector3::Transform(Vector3::Right, transformA));

		axes.push_back(Vector3::Transform(Vector3::Up, transformB));
		axes.push_back(Vector3::Transform(Vector3::Forward, transformB));
		axes.push_back(Vector3::Transform(Vector3::Right, transformB));

		for (int a = 0; a < 3; a++) {
			for (int b = 3; b < 6; b++) {
				Vector3 cross = axes[a].Cross(axes[b]);
				if (cross.LengthSquared() > 0)
					axes.push_back(cross);
			}
		}
		for (auto & axis : axes) {
			axis.Normalize();
		}
		return axes;
	}
	bool SatIntersection(std::vector<Vector3> & hullA, std::vector<Vector3> & hullB, std::vector<Vector3> axes, SatResult & result) {
		for (auto & axis : axes) {
			SatProjection projectionA = Project(axis, hullA);
			SatProjection projectionB = Project(axis, hullB);
			if (!projectionA.Overlaps(projectionB)) {
				result.Axis = axis;
				return false;
			}
		}
		return true;
	}
}