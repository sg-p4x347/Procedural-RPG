#pragma once
#include "pch.h"
#include "SimpleMath.h"

using namespace std;
using namespace DirectX;
using namespace SimpleMath;

namespace Utility {
	static const float sqrtTwo = 1.41421356237;
	float pythag(double x, double y, double z);
	float pythag(float x, float y);
	const double M_E = 2.71828182845904523536;
	int posToIndex(int x, int y, int width);
	int indexToX(int index, int width);
	int indexToY(int index, int width);
	// Randomness
	float randWithin(float min, float max);
	int randWithin(int min, int max);
	float Deviation(float range, float offset);
	bool Chance(float percent);
	int Probability(vector<float> probabilities);
	int sign(int integer);

	template<typename T>
	inline void InitArray(T * array,const int & size,const T & value) {
		for (int i = 0; i < size; i++) {
			array[i] = value;
		}
	}
	float Gaussian(float x, float a, float b, float c); // a controls amplituide; b controls x displacement; c controls width
	float Sigmoid(float x, float a, float b, float c); // a controls amplituide; b controls x displacement; c controls width
	float Inverse(float x, float a, float b, float c); // a controls amplituide; b controls x displacement; c controls width
	void Normalize(std::vector<float> & set);
	float InterpolateQuad(float x, float y, float bl, float br, float tr, float tl);
}

