#pragma once

using namespace std;

struct HeightMap
{
	HeightMap();
	HeightMap(int w, float i, float d, int z);
	vector< vector<float> > map;
	int width;
	float initialDeviation;
	float diamondDeviation;
	float squareDeviation;
	float deviationDecrease;
	int zoom;

	HeightMap Submap(int x, int y, int width);
};

