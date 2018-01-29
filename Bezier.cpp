#include "pch.h"
#include "Bezier.h"



Bezier::Bezier(vector<Vector3> controlPoints) : m_controlPoints(controlPoints), m_order(controlPoints.size()-1)
{
	// Cache the binomial coefficients
	for (int i = 0; i <= m_order; i++) {
		m_binomialCoefficients.push_back(Utility::BinomialCoefficient(m_order, i));
	}
}

Vector3 Bezier::GetPoint(float t)
{
	Vector3 result = Vector3::Zero;
	for (int i = 0; i <= m_order; i++) {
		float coefficient = m_binomialCoefficients[i] * std::pow((1 - t), m_order - i) * std::pow(t, i);
		result += coefficient * m_controlPoints[i];
	}
	return result;
}

Bezier & Bezier::GetDerivative()
{
	if (!m_derivative) {
		// derivative control points = n(P1 - P0), n(P2 - P1), ... n(Pn - Pn-1)
		vector<Vector3> controlPoints;
		for (int i = 0; i < m_order; i++) {
			controlPoints.push_back(m_controlPoints[i + 1] - m_controlPoints[i]);
		}
		m_derivative = std::make_unique<Bezier>(controlPoints);
	}
	return *m_derivative;
}
