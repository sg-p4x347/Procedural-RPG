#pragma once

#include "ShaderStructures.h"
#include "Utility.h"
#include "pch.h"

using namespace DirectX;
using namespace Utility;

class Distribution {
	public:
		Distribution (float maxDeviation, const float deviationDecrease, const int zoom, const bool isMountain);
		Distribution (float maxDev, const float devDecrease, const int zoom, Distribution *continentMap,  Distribution *biomeMap);
		int getWidth();
		// members
		float points[1025][1025];
		~Distribution();
	private:
		// members
		float input_maxDeviation;
		float input_deviationDecrease;
		int input_zoom;
		bool input_isMountain;
		unsigned short mapWidth = 1024;
		// algorithm
		float diamond(int x, int y, int distance);
		float square(int x, int y, int distance);
		float deviation(float range);
		// for continents
		float biomeDeviation(float zValue, int iteration, int zoom);
};

