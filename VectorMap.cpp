#include "pch.h"
#include "VectorMap.h"


VectorMap::VectorMap()
{
}

VectorMap::VectorMap(int width)
{
	Map = vector< vector<DirectX::SimpleMath::Vector3> >(width + 1, vector<DirectX::SimpleMath::Vector3>(width + 1));
}

