#include "pch.h"
#include "Utility.h"
using namespace DirectX;

namespace Utility {
	float randWithin(float min, float max) {
		return (float(rand()) / float(RAND_MAX))*(max - min) + min;
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
		if (integer != 0) {
			return abs(integer) / integer;
		}
		else {
			return 0;
		}
	}

	float pythag(double x, double y, double z) {
		return sqrt(x*x + y*y + z*z);
	}
	float pythag(float x, float y)
	{
		return sqrt(x*x + y*y);
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
		return floor(index / width);
	}
	void StringToWchar(string input, wchar_t * output)
	{
		MultiByteToWideChar(CP_UTF8, 0, input.c_str(), input.size(),output,-1);
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

	void Normalize(std::vector<float> & set)
	{
		float sum = 0.f;
		for (float value : set) {
			sum += std::abs(value);
		}
		if (sum > 0.f) {
			for (int i = 0; i < set.size(); i++) {
				set[i] /= sum;
			}
		}
	}

	float InterpolateQuad(float x, float y, float bl, float br, float tr, float tl)
	{
		vector<float> distances{ std::max(0.f,1.f-pythag(x,y)) , std::max(0.f,1.f - pythag(1 - x,y)), std::max(0.f,1.f - pythag(1 - x,1 - y)), std::max(0.f,1.f - pythag(x,1 - y)) };
		Utility::Normalize(distances);

		return ((distances[0]) * bl +( distances[1]) * br + (distances[2]) * tr +(distances[3]) * tl);
	}

	float BicubicInterpolation(float p[4][4], float x, float y)
	{
		float arr[4];
		arr[0] = CubicInterpolate(p[0], y);
		arr[1] = CubicInterpolate(p[1], y);
		arr[2] = CubicInterpolate(p[2], y);
		arr[3] = CubicInterpolate(p[3], y);
		return CubicInterpolate(arr, x);
	}
	float CubicInterpolate(float p[4], float x) {
		return p[1] + 0.5 * x*(p[2] - p[0] + x*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x*(3.0*(p[1] - p[2]) + p[3] - p[0])));
	}
}