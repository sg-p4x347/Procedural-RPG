#pragma once
#include "ISerialization.h"
using namespace std;
template <typename DataType>
struct Map :
	public ISerialization
{
	Map() : Map::Map(0, 0.0, 0.0, 0) {}
	Map(int w, float i, float d, int z) : Map::Map(w,w)
	{
		width = w;
		initialDeviation = i;
		diamondDeviation = i;
		squareDeviation = i;
		deviationDecrease = d;
		zoom = z;
	}
	Map(int width, int length = width);
	Map(Rectangle area);
	DataType & ValueAt(int x, int y);
	DataType default = DataType();
	vector< vector<DataType> > map;
	int width;
	int length;
	Rectangle area;
	float initialDeviation;
	float diamondDeviation;
	float squareDeviation;
	float deviationDecrease;
	int zoom;

	bool Bounded(int x, int y);
	bool Bounded(float x, float y);

	virtual void Resize(int width, int length = width) {
		map = vector<vector<DataType>>(width, vector<DataType>(length));
	}
	// Inherited via ISerialization
	virtual void Import(std::ifstream & ifs) override
	{
		DeSerialize(width, ifs);
		DeSerialize(length, ifs);
		Resize(width + 1, length + 1);
	}
	
	virtual void Export(std::ofstream & ofs) override
	{
		Serialize(width, ofs);
		Serialize(length, ofs);
	}
};

template<typename DataType>
inline Map<DataType>::Map(int width, int length) : Map<DataType>::Map(SimpleMath::Rectangle(0,0,width,length))
{
	assert(std::abs(width * length) < 100000000);
}

template<typename DataType>
inline Map<DataType>::Map(SimpleMath::Rectangle area) : area(area), width(area.width), length(area.height)
{
	Resize(area.width + 1, area.height + 1);
}

template<typename DataType>
inline DataType & Map<DataType>::ValueAt(int x, int y)
{
	if (area.x <= x && area.x + area.width >= x && area.y <= y && area.y + area.height >= y) {
		return map[x - area.x][y - area.y];
	}
	return default;
}

template<typename DataType>
inline bool Map<DataType>::Bounded(int x, int y)
{
	return x >= 0 && x <= width && y >= 0 && y <= length;
}
template<typename DataType>
inline bool Map<DataType>::Bounded(float x, float y)
{
	return x >= 0.f && x <= (float)width && y >= 0.f && y <= (float)length;
}
