#pragma once
#include "pch.h"
namespace geometry {
	Vector2 Clamp(Rectangle rect, Vector2 point);
	struct Int3 {
		Int3(int pX, int pY, int pZ);
		int x;
		int y;
		int z;
	};
}