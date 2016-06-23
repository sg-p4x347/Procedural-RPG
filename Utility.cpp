#include "pch.h"
#include "Utility.h"

namespace Utility {
	float randWithin(float min, float max) {
		return (float(rand()%1000)/1000.0)*(max - min) + min;
	}
	float pythag(float x, float y, float z) {
		return sqrt(x*x + y*y + z*z);
	}
	int posToIndex(int x, int y, int width) {
		return (x + (y*width));
	}
	int indexToX(int index, int width)
	{
		int y = floor(index / width);
		int x = index - (y * width);
		return x;
	}
	int indexToY(int index, int width)
	{
		return floor(index/width);
	}
}