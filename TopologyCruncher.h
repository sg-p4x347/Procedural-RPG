#pragma once
#include "PositionNormalTextureTangentColorVBO.h"
#include "MeshPart.h"
enum PathType {
	BezierPath,
	CubicPath,
	LinearPath
};
class TopologyCruncher
{
public:
	TopologyCruncher();
	~TopologyCruncher();
	//----------------------------------------------------------------
	// Returns a Vertex Buffer Object (VBO) representing all sub meshes
	Components::PositionNormalTextureTangentColorVBO CreateVBO();
	//----------------------------------------------------------------
	// Returns a geometry::MeshPart instance
	geometry::MeshPart CreateMeshPart();

	//----------------------------------------------------------------
	// One dimensional

	// Creates a flat triangle strip along a cubic spline defined by a vector of points
	// A second vector of normals gives the orientation of the strip
	void Strip(
		vector<Vector3> & path, 
		std::function<float(float & t)> roll = [](float & t) {return HALF_PI;},
		std::function<float(float & t)> width = [](float & t) {return 1.f;},
		int divisions = 10
	);
	void Tube(
		vector<Vector3> & path,
		std::function<float(float & t)> diameter = [](float & t) {return 1.f; },
		int longitudeDivisions = 10,
		int radialDivisions = 10,
		PathType type = PathType::BezierPath
	);

private:
	VertexPositionNormalTangentColorTexture CreateVertex(Vector3 position, Vector3 normal, Vector2 texture);
	vector<VertexPositionNormalTangentColorTexture> m_VB;
	vector<uint16_t> m_IB;
};

//----------------------------------------------------------------
// Usage examples

//{
//	// Strip
//	vector<Vector3> path = vector<Vector3>{
//		Vector3(0,0,0),
//		Vector3(5.f,10.f,5.f),
//		Vector3(10.f,5.f,10.f),
//		Vector3(20.f,20.f,20.f)
//	};
//	topologyCruncher.Strip(path, [](float & t) {
//		return t * 5.f;
//	}, [](float & t) {
//		float ends[2]{ 1.f,0.1f };
//		return Utility::LinearInterpolate(ends, t);
//	},60);
//}
//{
//	// Tube
//	vector<Vector3> path = vector<Vector3>{
//		Vector3(0,0,0),
//		Vector3(5,0,0),
//		Vector3(5,0,5),
//		Vector3(0,0,5),
//		Vector3(0,0,0)
//	};
//	topologyCruncher.Tube(path, [](float & t) {
//		return 2.f;
//	}, 50,20,PathType::CubicPath);
//}

