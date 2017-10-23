#include "pch.h"
#include "HeightMap.h"


HeightMap::HeightMap()
{
	HeightMap(0, 0.f, 0.f, 0);
}

HeightMap::HeightMap(int w, float i, float d, int z)
{
	width = w;
	initialDeviation = i;
	diamondDeviation = i;
	squareDeviation = i;
	deviationDecrease = d;
	zoom = z;
	map = vector< vector<float> >(width + 1, vector<float>(width + 1));
}

HeightMap HeightMap::Submap(int x, int y, int width)
{
	return HeightMap(width, initialDeviation, initialDeviation, zoom);
	
}
