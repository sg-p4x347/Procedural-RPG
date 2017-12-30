#pragma once

using namespace std;
template <typename DataType>
struct Map
{
	Map() : Map::Map(0, 0.0, 0.0, 0) {}
	Map(int w, float i, float d, int z)
	{
		width = w;
		initialDeviation = i;
		diamondDeviation = i;
		squareDeviation = i;
		deviationDecrease = d;
		zoom = z;
		map = vector< vector<DataType> >(width + 1, vector<DataType>(width + 1));
	}
	vector< vector<DataType> > map;
	int width;
	float initialDeviation;
	float diamondDeviation;
	float squareDeviation;
	float deviationDecrease;
	int zoom;

	bool Bounded(int x, int y);
	bool Bounded(float x, float y);
};

template<typename DataType>
inline bool Map<DataType>::Bounded(int x, int y)
{
	return x >= 0 && x <= width && y >= 0 && y <= width;
}
template<typename DataType>
inline bool Map<DataType>::Bounded(float x, float y)
{
	return x >= 0.f && x <= (float)width && y >= 0.f && y <= (float)width;
}
