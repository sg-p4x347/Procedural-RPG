#pragma once
#include "PositionNormalTextureVBO.h"

class TopologyCruncher
{
public:
	TopologyCruncher();
	~TopologyCruncher();
	//----------------------------------------------------------------
	// Returns a Vertex Buffer Object (VBO) representing all sub meshes
	Components::PositionNormalTextureVBO CreateVBO();

	//----------------------------------------------------------------
	// One dimensional

	// Creates a flat triangle strip along a cubic spline defined by a vector of points
	// A second vector of normals gives the orientation of the strip
	void Strip(
		vector<Vector3> & path, 
		std::function<float(float & t)> roll = [](float & t) {return HALF_PI;},
		std::function<float(float & t)> width = [](float & t) {return 1.f;},
		float segmentLength = 0.1f
	);
private:
	vector<VertexPositionNormalTexture> m_VB;
	vector<uint16_t> m_IB;
};

