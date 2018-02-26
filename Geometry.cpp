#include "pch.h"
#include "Geometry.h"
namespace Geom {
	Vector2 Clamp(Rectangle rect, Vector2 point)
	{
		return Vector2(Utility::Clamp(point.x, rect.x, rect.x + rect.width), Utility::Clamp(point.y, rect.y, rect.y + rect.height));
	}
}
