#pragma once
#include "pch.h"

namespace Utility {
	float randWithin(float min, float max);
	float pythag(float x, float y, float z);
	struct Float4 {
		float x;
		float y;
		float z;
		float w;
	};
	const double M_E = 2.71828182845904523536;
	int posToIndex(int x, int y, int width);
	int indexToX(int index, int width);
	int indexToY(int index, int width);
}

