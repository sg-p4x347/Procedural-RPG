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
	Vector3 previousLeft;
	Vector3 previousRight;
	for (int i = 0; i <= divisions; i++) {
		Bezier bezierCurve = Bezier(path);
		Bezier & derivative = bezierCurve.GetDerivative();
		
		Vector3 point = bezierCurve.GetPoint(t);

		Vector3 forward = derivative.GetPoint(t);
		forward.Normalize();
		/*float yaw = std::asin(forward.z);
		float pitch = std::asin(forward.y);*/
/*
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
		);*/

		//Vector3 left = Vector3(-cosY * sinP*sinR - sinY * cosR, -sinY * sinP*sinR + cosY * cosR, cosP*sinR);
		Vector3 left = previousLeft;
		Vector3 right = previousRight;
		if (std::abs(forward.x) > 0.0001f || std::abs(forward.z) > 0.0001f) {
			left = Vector3(forward.x, 0, -forward.z);
			left.Normalize();
			previousLeft = left;
			Vector3 right = Vector3(-forward.x, 0, forward.z);
			right.Normalize();
			previousRight = right;
		}
		
		Vector3 normal = forward.Cross(left);

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
	int longitudeDivisions,
	int radialDivisions)
{
	const float dt = 1.f / (float)longitudeDivisions;

	vector<vector<Vector3>> loops;
	vector<Vector3> points;
	float t = 0.f;
	Vector3 previousLeft;
	for (int i = 0; i <= longitudeDivisions; i++) {
		Bezier bezierCurve = Bezier(path);
		Bezier & derivative = bezierCurve.GetDerivative();

		Vector3 point = bezierCurve.GetPoint(t);
		points.push_back(point);
		Vector3 forward = derivative.GetPoint(t);
		forward.Normalize();

		//Vector3 left = Vector3(-cosY * sinP*sinR - sinY * cosR, -sinY * sinP*sinR + cosY * cosR, cosP*sinR);

		Vector3 left = previousLeft;
		if (std::abs(forward.x) > 0.0001f || std::abs(forward.z) > 0.0001f) {
			left = Vector3(forward.x, 0, -forward.z);
			left.Normalize();
			previousLeft = left;
		}

		Vector3 normal = forward.Cross(left);

		float radius = diameter(t) / 2.f;
		vector<Vector3> loop;
		float theta = 0.f;
		float dTheta = TWO_PI / (float)radialDivisions;
		for (int j = 0; j < radialDivisions; j++) {
			/*float cosT = cos(theta);
			float a = 1 - cosT;
			float sinT = sin(theta);*/

			/*XMMATRIX rotation = (
				cosT + forward.x * forward.x * a,forward.x * forward.y * a - forward.z*sinT,forward.x * forward.z * a + forward.y * sinT,
				forward.y * forward.x * a + forward.z * sinT,	cosT + forward.y *forward.y * a,forward.y * forward.z * a - forward.x*sinT,
				forward.z * forward.x * a - forward.y * sinT,	forward.z * forward.y * a + forward.x * sinT,	cosT + forward.z * forward.z * a
			);*/

			XMMATRIX rotation = XMMatrixRotationNormal(forward, theta);
			//XMMATRIX translation = XMMatrixTranslation(left.x, left.y, left.z);
			//XMMATRIX final = XMMatrixMultiply(rotation, translation);
			Vector3 rotated = Vector3::Transform(left, rotation);
			loop.push_back(point + radius * rotated);
			theta += dTheta;
		}

		loops.push_back(loop);

		t += dt;
	}
	// make quads
	for (int loopIndex = 0; loopIndex < longitudeDivisions; loopIndex++) {
		vector<Vector3> loop = loops[loopIndex];
		vector<Vector3> nextLoop = loops[loopIndex +1];
		for (int radialIndex = 0; radialIndex < radialDivisions; radialIndex++) {

			/*
			l = longitude index
			r = radial index
			2(l+1,r)---3(l+1,r+1)
			|       \			|
			1(l,r)-------4(l,r+1)
			*/
			Vector3 v1 = loop[radialIndex];
			Vector3 n1 = (v1 - points[loopIndex]);
			n1.Normalize();
			Vector3 v2 = nextLoop[radialIndex];
			Vector3 n2 = (v2 - points[loopIndex + 1]);
			n2.Normalize();
			Vector3 v3 = nextLoop[(radialIndex + 1) % radialDivisions];
			Vector3 n3 = (v3 - points[loopIndex + 1]);
			n3.Normalize();
			Vector3 v4 = loop[(radialIndex + 1) % radialDivisions];
			Vector3 n4 = (v4 - points[loopIndex]);
			n4.Normalize();


			m_VB.push_back(VertexPositionNormalTexture(v2, n2, Vector2(0, 0))); // 1
			m_VB.push_back(VertexPositionNormalTexture(v4,n4, Vector2(1.f, 0))); // 2
			m_VB.push_back(VertexPositionNormalTexture(v1, n1, Vector2(1.f, 1.f))); // 4

			m_VB.push_back(VertexPositionNormalTexture(v2, n2, Vector2(0, 0))); // 1
			m_VB.push_back(VertexPositionNormalTexture(v3, n3, Vector2(1.f, 1.f))); // 4
			m_VB.push_back(VertexPositionNormalTexture(v4, n4, Vector2(0, 1.f))); // 3

			m_IB.push_back(m_IB.size());
			m_IB.push_back(m_IB.size());
			m_IB.push_back(m_IB.size());
			m_IB.push_back(m_IB.size());
			m_IB.push_back(m_IB.size());
			m_IB.push_back(m_IB.size());
		}
	}
}
