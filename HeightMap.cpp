#include "pch.h"
#include "HeightMap.h"
#include "Utility.h"

using Vector2 = DirectX::SimpleMath::Vector2;
HeightMap::HeightMap() : Map<float>::Map()
{
}

HeightMap::HeightMap(int w, float i, float d, int z) : Map<float>::Map(w,i,d,z)
{
}

DirectX::SimpleMath::Vector2 HeightMap::GradientDirection(float x, float z)
{
	if (Bounded(x, z)) {
		/*
		z
		|
		D +  + C
		+(x+u, +
		+  z+v)+
		A +  + B -- x
		*/
		int left = std::floor(x);
		int bottom = std::floor(z);
		int right = std::ceil(x);
		int top = std::ceil(z);

		float u = x - (float)left;
		float v = z - (float)bottom;

		float a = map[left][bottom];
		float b = map[right][bottom];
		float c = map[right][top];
		float d = map[left][top];


		return Vector2(
			(b - a) * (1 - v) + (c - d)*v,
			(d - a) * (1 - u) + (c - b)*u
		);
	}
	return Vector2::Zero;
}

DirectX::SimpleMath::Vector2 HeightMap::GradientDirection(int x, int z)
{
	/*
		   z
		   |
		  top
		   |
	left-center-right---x
		   |
		 bottom
	*/
	if (Bounded(x, z)) {
		float center = map[x][z];
		float top = z < width ? map[x][z+1] : center;
		float bottom = z > 0 ? map[x][z-1] : center;
		float right = x < width ? map[x+1][z] : center;
		float left = x > 0 ? map[x-1][z] : center;
		return Vector2(
			((right - center) + (center - left)) / 2.f,
			((top - center) + (center - bottom)) / 2.f
		);
	}
	return Vector2::Zero;
}

float HeightMap::GradientAngle(float x, float z)
{
	return std::atan(GradientDirection(x, z).Length());
}

float HeightMap::GradientAngle(int x, int z)
{
	return std::atan(GradientDirection(x, z).Length());
}

float HeightMap::Height(float x, float z)
{
	if (std::floorf(x) == x && std::floorf(z) == z) {
		return Height((int)x, (int)z);
	}
	else if (x < 0.f || z < 0.f || x > width || z > width) {
		return 0.f;
	}
	else {
		float quad[2][2]{
			{map[std::floor(x)][std::floor(z)],map[std::floor(x)][std::ceil(z)]},
			{map[std::ceil(x)][std::floor(z)],map[std::ceil(x)][std::ceil(z)]}
		};
		return Utility::BilinearInterpolate(quad, x - std::floor(x), z - std::floor(z));
	}
}

float HeightMap::Height(int x, int z)
{
	if (Bounded(x,z)) {
		return map[x][z];
	}
	return 0.0f;
}

