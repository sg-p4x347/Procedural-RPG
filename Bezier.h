#pragma once
class Bezier
{
public:
	Bezier(vector<Vector3> controlPoints);
	//----------------------------------------------------------------
	// Get point on the curve with respect to t (0.0 -> 1.0)
	Vector3 GetPoint(float t);
	//----------------------------------------------------------------
	// Get derivative value of the curve with respect to t (0.0 -> 1.0)
	Bezier & GetDerivative();
private:
	//----------------------------------------------------------------
	// Cache common values
	const int m_order;
	vector<Vector3> m_controlPoints;
	vector<float> m_binomialCoefficients;

	//----------------------------------------------------------------
	// Derivative (order - 1) Bezier curve
	std::unique_ptr<Bezier> m_derivative;
};

