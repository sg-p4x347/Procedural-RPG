#include "pch.h"
#include "Geometry.h"
namespace geometry {
	Vector2 Clamp(Rectangle rect, Vector2 point)
	{
		return Vector2(Utility::Clamp(point.x, rect.x, rect.x + rect.width), Utility::Clamp(point.y, rect.y, rect.y + rect.height));
	}
	Int3::Int3(int pX, int pY, int pZ) : x(pX), y(pY), z(pZ)
	{
	}
	Int3 Int3::operator-(Int3 & other)
	{
		return Int3(x - other.x, y - other.y, z - other.z);
	}
	Int3 Int3::operator+(Int3 & other)
	{
		return Int3(x + other.x, y + other.y, z + other.z);
	}
}
