#pragma once

#include "Utility.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace Utility;

class Distribution {
	public:
		Distribution();
		Distribution(const unsigned int width, const unsigned int height);
		~Distribution();
		// Generation
		void DiamondSquare(float maxDeviation, const float deviationDecrease, const int zoom, const bool isMountain);
		/*
		Creates a continent heightmap
		@param: what resolution to start interpolating points
		*/
		void Continent(int ctrlWidth);
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
		unsigned int m_height;
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
		// Erosion filter for generated terrain
		
		float minimum_distance(Vector2 v, Vector2 w, Vector2 p);
};