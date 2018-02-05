#pragma once
#include "Bezier.h"
class CubicSpline
{
public:
	CubicSpline(vector<Vector3> controlPoints,float tightness = 1.f,int subDivisions = 10);
	//----------------------------------------------------------------
	// Get point on the curve with respect to t (0.0 -> 1.0)
	Vector3 GetPoint(float t);
	//----------------------------------------------------------------
	// Get derivative value of the curve with respect to t (0.0 -> 1.0)
	CubicSpline & GetDerivative(int subDivisions = 10);
	//----------------------------------------------------------------
	// Get approximate length
	float Length();
private:
	CubicSpline(vector<std::pair<Bezier, float>> bezierSections);
	vector<std::pair<Bezier, float>> m_bezierSections;
	float m_length;
	unique_ptr<CubicSpline> m_derivative;
};

