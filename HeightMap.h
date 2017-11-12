#pragma once

using namespace std;
template <typename DataType>
struct HeightMap
{
	HeightMap() : HeightMap::HeightMap(0, 0.0, 0.0, 0) {}
	HeightMap(int w, float i, float d, int z)
	{
		width = w;
		initialDeviation = i;
		diamondDeviation = i;
		squareDeviation = i;
		deviationDecrease = d;
		zoom = z;
		Map = vector< vector<DataType> >(width + 1, vector<DataType>(width + 1));
	}
	vector< vector<DataType> > Map;
	int width;
	float initialDeviation;
	float diamondDeviation;
	float squareDeviation;
	float deviationDecrease;
	int zoom;

	HeightMap Submap(int x, int y, int width);
};

