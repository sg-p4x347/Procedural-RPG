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
}