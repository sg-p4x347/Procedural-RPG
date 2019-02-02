#include "pch.h"
#include "HeightMap.h"
#include "Utility.h"

using Vector2 = DirectX::SimpleMath::Vector2;
HeightMap::HeightMap() : Map<float>::Map()
{
}

HeightMap::~HeightMap()
{
}

HeightMap::HeightMap(int w, float i, float d, int z) : Map<float>::Map(w,i,d,z)
{
}

HeightMap::HeightMap(int width, int length) : Map<float>::Map(width,length)
{
}

HeightMap::HeightMap(Rectangle area) : Map<float>::Map(area)
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
	else if (x < 0.f || z < 0.f || x > width || z > length) {
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

float HeightMap::Height(float x, float z, std::function<float(int, int)>&& controlSelector)
{
	if (std::floorf(x) == x && std::floorf(z) == z) {
		return controlSelector((int)x, (int)z);
	}
	else {
		float quad[2][2]{
			{controlSelector(std::floor(x),std::floor(z)),controlSelector(std::floor(x),std::ceil(z))},
			{controlSelector(std::ceil(x),std::floor(z)),controlSelector(std::ceil(x),std::ceil(z))}
		};
		return Utility::BilinearInterpolate(quad, x - std::floor(x), z - std::floor(z));
	}
}

Vector3 HeightMap::Normal(int x, int z, std::function<float(int, int)>&& controlSelector)
{
	float left = controlSelector(x - 1, z);
	float right = controlSelector(x + 1, z);
	float down = controlSelector(x, z - 1);
	float up = controlSelector(x, z + 1);

	Vector3 normal = DirectX::SimpleMath::Vector3(left - right, 2.f, down - up);
	normal.Normalize();
	return normal;
}

float HeightMap::Height(int x, int z)
{
	if (Bounded(x,z)) {
		return map[x][z];
	}
	return 0.0f;
}

void HeightMap::SetHeight(int x, int z, float value)
{
	if (Bounded(x, z)) {
		map[x][z] = value;
	}
}

float HeightMap::HeightAbsPos(Vector2 position)
{
	return Height(position.x - (float)area.x, position.y - (float)area.y);
}

