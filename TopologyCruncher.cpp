#include "pch.h"
#include "TopologyCruncher.h"
#include "Bezier.h"

TopologyCruncher::TopologyCruncher()
{
}


TopologyCruncher::~TopologyCruncher()
{
}

Components::PositionNormalTextureVBO TopologyCruncher::CreateVBO()
{
	auto vbo = Components::PositionNormalTextureVBO();
	vbo.Vertices = m_VB;
	vbo.Indices = m_IB;
	return vbo;
}



void TopologyCruncher::Strip(vector<Vector3>& path, std::function<float(float&t)> roll, std::function<float(float&t)> width, int divisions)
{
	//----------------------------------------------------------------
	// Bezier curve and its derivative: https://pages.mtu.edu/~shene/COURSES/cs3621/NOTES/spline/Bezier/bezier-der.html

	const float dt = 1.f / (float)divisions;

	vector<Vector3> leftEdge;
	vector<Vector3> rightEdge;
	vector<Vector3> normals;
	float t = 0.f;
	for (int i = 0; i <= divisions; i++) {
		Bezier bezierCurve = Bezier(path);
		Bezier & derivative = bezierCurve.GetDerivative();
		
		Vector3 point = bezierCurve.GetPoint(t);

		Vector3 forward = derivative.GetPoint(t);
		forward.Normalize();
		float yaw = std::asin(forward.z);
		float pitch = std::asin(forward.y);

		float sinY = sin(yaw);
		float cosY = cos(yaw);
		float sinP = sin(pitch);
		float cosP = cos(pitch);
		float sinR = -sin(roll(t));
		float cosR = cos(roll(t));
		XMFLOAT3X3 rotation = XMFLOAT3X3(
			cosY * cosP,	-cosY * sinP*sinR - sinY * cosR,	-cosY * sinP*cosR + sinY * sinR,
			sinY*cosP,		-sinY * sinP*sinR + cosY * cosR,	-sinY * sinP*cosR - cosY * sinR,
			sinP,			cosP*sinR,							cosP*sinR
		);

		Vector3 left = Vector3(-cosY * sinP*sinR - sinY * cosR, -sinY * sinP*sinR + cosY * cosR, cosP*sinR);
		Vector3 normal = forward.Cross(left);
		Vector3 right = forward.Cross(normal);

		float radius = width(t) / 2.f;

		leftEdge.push_back(left * radius + point);
		rightEdge.push_back(right * radius + point);
		normals.push_back(normal);

		t += dt;
	}
	// make quads
	for (int i = 0; i < divisions; i++) {
		/*
		1---2
		| \ |
		3---4
		*/
		m_VB.push_back(VertexPositionNormalTexture(leftEdge[i + 1], normals[i + 1], Vector2(0, 0))); // 1
		m_VB.push_back(VertexPositionNormalTexture(rightEdge[i + 1], normals[i + 1], Vector2(1.f, 0))); // 2
		m_VB.push_back(VertexPositionNormalTexture(rightEdge[i], normals[i], Vector2(1.f, 1.f))); // 4

		m_VB.push_back(VertexPositionNormalTexture(leftEdge[i + 1], normals[i + 1], Vector2(0, 0))); // 1
		m_VB.push_back(VertexPositionNormalTexture(rightEdge[i], normals[i], Vector2(1.f, 1.f))); // 4
		m_VB.push_back(VertexPositionNormalTexture(leftEdge[i], normals[i], Vector2(0, 1.f))); // 3

		m_IB.push_back(m_IB.size());
		m_IB.push_back(m_IB.size());
		m_IB.push_back(m_IB.size());
		m_IB.push_back(m_IB.size());
		m_IB.push_back(m_IB.size());
		m_IB.push_back(m_IB.size());
	}
}

void TopologyCruncher::Tube(
	vector<Vector3>& path, 
	std::function<float(float&t)> diameter,
	int longitudeDivisions = 10,
	int radialDivisions = 10)
{
	const float dt = 1.f / (float)longitudeDivisions;

	vector<Vector3> leftEdge;
	vector<Vector3> rightEdge;
	vector<Vector3> normals;
	float t = 0.f;
	for (int i = 0; i <= longitudeDivisions; i++) {
		Bezier bezierCurve = Bezier(path);
		Bezier & derivative = bezierCurve.GetDerivative();

		Vector3 point = bezierCurve.GetPoint(t);

		Vector3 forward = derivative.GetPoint(t);
		forward.Normalize();

		Vector3 left = Vector3(-cosY * sinP*sinR - sinY * cosR, -sinY * sinP*sinR + cosY * cosR, cosP*sinR);
		Vector3 normal = forward.Cross(left);
		Vector3 right = forward.Cross(normal);

		float radius = width(t) / 2.f;

		leftEdge.push_back(left * radius + point);
		rightEdge.push_back(right * radius + point);
		normals.push_back(normal);

		t += dt;
	}
	// make quads
	for (int i = 0; i < divisions; i++) {
		/*
		1---2
		| \ |
		3---4
		*/
		m_VB.push_back(VertexPositionNormalTexture(leftEdge[i + 1], normals[i + 1], Vector2(0, 0))); // 1
		m_VB.push_back(VertexPositionNormalTexture(rightEdge[i + 1], normals[i + 1], Vector2(1.f, 0))); // 2
		m_VB.push_back(VertexPositionNormalTexture(rightEdge[i], normals[i], Vector2(1.f, 1.f))); // 4

		m_VB.push_back(VertexPositionNormalTexture(leftEdge[i + 1], normals[i + 1], Vector2(0, 0))); // 1
		m_VB.push_back(VertexPositionNormalTexture(rightEdge[i], normals[i], Vector2(1.f, 1.f))); // 4
		m_VB.push_back(VertexPositionNormalTexture(leftEdge[i], normals[i], Vector2(0, 1.f))); // 3

		m_IB.push_back(m_IB.size());
		m_IB.push_back(m_IB.size());
		m_IB.push_back(m_IB.size());
		m_IB.push_back(m_IB.size());
		m_IB.push_back(m_IB.size());
		m_IB.push_back(m_IB.size());
	}
}
