#pragma once
#include "Line.h"
struct Box
{
	Box();
	Box(Vector3 position, Vector3 size);
	Vector3 Position;
	Vector3 Size;

	bool Contains(Vector3 point);
	bool Intersects(Line segment, int tests = 2);
	
};

