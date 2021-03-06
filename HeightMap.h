#pragma once
#include "Map.h"
class HeightMap :
	public Map<float>
{
public:
	HeightMap();
	~HeightMap();
	HeightMap(int w, float i, float d, int z);
	HeightMap(int width, int length);
	HeightMap(Rectangle area);
	DirectX::SimpleMath::Vector2 GradientDirection(float x, float z);
	DirectX::SimpleMath::Vector2 GradientDirection(int x, int z);
	float GradientAngle(float x, float z);
	float GradientAngle(int x, int z);
	float Height(float x, float z);
	static float Height(float x, float z, std::function<float(int, int)> && controlSelector);
	static Vector3 Normal(int x, int z, std::function<float(int, int)> && controlSelector);
	float Height(int x, int z);
	void SetHeight(int x, int z, float value);
	float HeightAbsPos(Vector2 position);
};

