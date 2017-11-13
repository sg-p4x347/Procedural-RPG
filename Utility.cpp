#include "pch.h"
#include "Utility.h"
using namespace DirectX;

namespace Utility {
	float randWithin(float min, float max) {
		return (float(rand())/float(RAND_MAX))*(max - min) + min;
	}
	int randWithin(int min, int max) {
		return rand() % (max - min) + min;
	}
	bool Chance(float percent)
	{
		return randWithin(0.f, 1.f) <= percent;
	}
	int Probability(vector<float> probabilities)
	{
		float sum = 0.f;
		for (float prob : probabilities) {
			sum += prob;
		}
		// Normalize probabilites
		for (float prob : probabilities) {
			prob /= sum;
		}
		float random = randWithin(0.f, 1.f);
		float offset = 0.f;
		for (int i = 0; i < probabilities.size(); i++) {
			if (random >= offset && random <= probabilities[i] + offset) {
				return i;
			}
			offset += probabilities[i];
		}
	}
	int sign(int integer)
	{
		return abs(integer) / integer;
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
	int pnpoly(vector<Vector2> polygon, Vector2 test)
	{
		int i, j, c = 0;
		for (i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
			if (((polygon[i].y>test.y) != (polygon[j].y>test.y)) &&
				(test.x < (polygon[j].x - polygon[i].x) * (test.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x))
				c = !c;
		}
		return c;
	}
	float Deviation(float range, float offset) {
		return randWithin(-range * 0.5f + offset, range * 0.5f + offset);
	}
	float Gaussian(float x, float a, float b, float c) {
		return a * exp(-pow((x - b) / (2.f*c), 2));
	}
	float Sigmoid(float x, float a, float b, float c) {
		return a / (1.f + exp(-c * (x - b)));
	}

	float Inverse(float x, float a, float b, float c)
	{
		return a * (1 - 1 / (c * (x - b) + 1));
	}
}