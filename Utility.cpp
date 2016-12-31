#include "pch.h"
#include "Utility.h"

namespace Utility {
	float randWithin(float min, float max) {
		return (float(rand())/float(RAND_MAX))*(max - min) + min;
	}
	int randWithin(int min, int max) {
		return rand() % (max - min) + min;
	}
	float pythag(double x, double y, double z) {
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