#include "pch.h"
#include "Line.h"

Line::Line(Vector3 a, Vector3 b) :  A(a), B(b)
{
	
}
double Line::Length()
{
	return Vector3::Distance(A, B);
}
//Line Line::Overlap(Line & B)
//{
//	if (CoLinear(B)) {
//		double minAx, minBx, maxAx, maxBx;
//		double minAy, minBy, maxAy, maxBy;
//		minAx = min(get<0>(m_points).x, get<1>(m_points).x);
//		minAy = min(get<0>(m_points).y, get<1>(m_points).y);
//		minBx = min(get<0>(B.m_points).x, get<1>(B.m_points).x);
//		minBy = min(get<0>(B.m_points).y, get<1>(B.m_points).y);
//
//		double minX = max(minAx, minBx);
//		double minY = max(minAy, minBy);
//		double maxX = min(maxAx, maxBx);
//		double maxY = min(maxAy, maxBy);
//
//		if (minX > maxX || minY > maxY) return Line(Vector3(), Vector3()); // not overlapping
//		Vector3 minP(minX, minY, 0.0);
//		Vector3 maxP(maxX, maxY, 0.0);
//		return Line(minP, maxP);
//	}
//	return Line(Vector3(), Vector3());
//}
//bool Line::CoLinear(Line & B)
//{
//	Vector3 Anormalized = get<0>(m_points) - get<1>(m_points);
//	Anormalized.Normalize();
//	Anormalized.x = abs(Anormalized.x);
//	Anormalized.y = abs(Anormalized.y);
//	Anormalized.z = abs(Anormalized.z);
//	Vector3 Bnormalized = get<0>(B.m_points) - get<1>(B.m_points);
//	Bnormalized.Normalize();
//	Bnormalized.x = abs(Bnormalized.x);
//	Bnormalized.y = abs(Bnormalized.y);
//	Bnormalized.z = abs(Bnormalized.z);
//
//	return Anormalized == Bnormalized;
//}
//bool Line::SharedCoord(int x)
//{
//	return get<0>(m_points).x == (float)x || get<0>(m_points).y == (float)x
//		|| get<1>(m_points).x == (float)x || get<1>(m_points).y == (float)x;
//}
