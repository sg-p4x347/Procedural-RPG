#pragma once
struct VectorMap
{
	VectorMap();
	VectorMap(int width);
	int Width;
	vector < vector < DirectX::SimpleMath::Vector3 > > Map;
};

