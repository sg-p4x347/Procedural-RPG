#include "pch.h"
#include "CubicSpline.h"
#include "Bezier.h"
CubicSpline::CubicSpline(vector<Vector3> controlPoints, float tightness, int subDivisions) : m_length(0.f)
{
	Vector3 endTangent;
	for (int i = 0; i < controlPoints.size()-1; i++) {
		vector<Vector3> path;
		Vector3 start = controlPoints[i];
		Vector3 end = controlPoints[i+1];
		path.push_back(start);
		if (endTangent != Vector3::Zero) {
			// start tangent becomes the negative of the previous end tangent
			path.push_back(-endTangent + start);
		}
		
		
		if (i < controlPoints.size() - 2) {
			Vector3 next = controlPoints[i + 2];

			Vector3 line1 = start - end;
			Vector3 line2 = end - next;

			line1.Normalize();
			line2.Normalize();
			// average the two vectors to get the normal of reflection
			endTangent = Vector3((line1.x + line2.x) / 2.f, (line1.y + line2.y) / 2.f, (line1.z + line2.z) / 2.f);
			//endTangent.Normalize();
			endTangent *= tightness;
			path.push_back(endTangent + end);
		}
		path.push_back(end);
		Bezier bezier = Bezier(path);
		float length = bezier.Length(subDivisions);
		m_length += length;
		m_bezierSections.push_back(std::make_pair(Bezier(bezier), length));
	}
}

CubicSpline::~CubicSpline()
{
}

Vector3 CubicSpline::GetPoint(float t)
{
	// normalize t
	t = std::min(t, 1.f);
	// select the bezier spline to use
	float dt = 1.f / (float)m_bezierSections.size();

	int index = std::min((int)m_bezierSections.size()-1,(int)std::floor(t/dt));
	//index = 0;
	//index = m_bezierSections.size() - 1;
	//int index = 0;
	//if (m_length > 0.f) {
	//	while (dt < t) {
	//		if (dt + m_bezierSections[index].second / m_length >= t) break;
	//		dt += m_bezierSections[index].second / m_length;
	//		index++;
	//	}
	//	// transform t into the frame of this bezier curve segment
	//	t -= dt;
	//	t /= (m_bezierSections[index].second / m_length);
	//}
	return m_bezierSections[index].first.GetPoint((t - index * dt) / dt);
}

CubicSpline & CubicSpline::GetDerivative(int subDivisions)
{
	if (!m_derivative) {
		vector<std::pair<Bezier, float>> bezierSections;
		for (auto & section : m_bezierSections) {
			auto & derivative = section.first.GetDerivative();
			bezierSections.push_back(std::make_pair(std::move(derivative), derivative.Length(subDivisions)));
		}
		m_derivative.reset(new CubicSpline(bezierSections));
	}
	return *m_derivative;
}

float CubicSpline::Length()
{
	if (m_length == 0.f) {
		for (auto & section : m_bezierSections) {
			m_length += section.second;
		}
	}
	return m_length;
}

CubicSpline::CubicSpline(vector<std::pair<Bezier, float>> bezierSections) : m_bezierSections(bezierSections), m_length(0.f)
{
	
}
