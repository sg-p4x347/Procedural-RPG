#pragma once
#include "HeightMap.h"

using namespace DirectX::SimpleMath;
class Droplet
{
public:
	Droplet();
	void Reset(int width);
	void Reset(HeightMap & map);
	void Update(HeightMap & map);
	Vector2 Direction;
	Vector2 Position;
	float Velocity;
	float Water;
	float Sediment;
	static const float Radius;
};

