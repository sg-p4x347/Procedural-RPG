#pragma once
#include "pch.h"

namespace Utility {
	float randWithin(float min, float max);
	int randWithin(int min, int max);
	float pythag(double x, double y, double z);
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

