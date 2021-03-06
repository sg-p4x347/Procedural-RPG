#pragma once
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Rectangle;
//----------------------------------------------------------------
// Constants

// The Golen Ratio (B is to A, as B + A is to B)
const double GR = 1.61803398875;

namespace ProUtil {
	Rectangle GoldenRect(double maxArea);
	Rectangle RectWithin(Rectangle A, Rectangle B);
	Vector2 PointWithin(Rectangle A);
	float RandWithin(float min, float max);
	int RandWithin(int min, int max);
	int ToRandom(string input);
	Vector3 RandomVec3(float rx, float ry, float rz);
	Vector3 RandomVec3(float radius);
}