#pragma once

#include "Utility.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace Utility;

class Distribution {
	public:
		Distribution();
		Distribution(const unsigned int width);
		~Distribution();
		// Generation
		// Creates a diamond-square algorithm heightmap
		void DiamondSquare(
			float maxDeviation /*maximum range of values*/ , 
			const float deviationDecrease /*higher is smoother, lower is more rough*/,
			const int zoom /*scales the scope of the algorithm, delays the deviation decrease effect*/, 
			const bool isMountain /*sets the center point to the maximum altitude*/);
		// Creates a continent heightmap
		void Continent(int ctrlWidth /*what resolution to start interpolating points*/);
		// filter simulates water erosion over time
		void Erosion();
		// Getters
		vector< vector<float> > GetPoints();
		unsigned int GetWidth();
	private:
		// members
		vector< vector<float> > m_points;
		float m_maxDeviation;
		float m_deviationDecrease;
		unsigned int m_zoom;
		bool m_mountain;
		unsigned int m_width;
		// Diamond Square Algorithm -------------------------------------
		float Diamond(int x, int y, int distance);
		float Square(int x, int y, int distance);
		float Deviation(float range);
		// Interpolation ------------------------------------------------
		float CubicInterpolate(float p[4], float x);
		float BicubicInterpolate(float p[4][4], float x, float y);
		// for continents -----------------------------------------------
		float BiomeDeviation(float biome, float continent);
		float Gaussian(float x, float a, float b, float c);
		float Sigmoid(float x, float a, float b, float c);
		
		float minimum_distance(Vector2 v, Vector2 w, Vector2 p);
};