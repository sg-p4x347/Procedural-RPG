#pragma once
#include "Map.h"
class HeightMap :
	public Map<float>
{
public:
	HeightMap();
	HeightMap(int w, float i, float d, int z);
	DirectX::SimpleMath::Vector2 GradientDirection(float x, float z);
	DirectX::SimpleMath::Vector2 GradientDirection(int x, int z);
	float GradientAngle(float x, float z);
	float GradientAngle(int x, int z);
	float Height(float x, float z);
	float Height(int x, int z);
};

