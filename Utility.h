#pragma once
#include "pch.h"
#include "SimpleMath.h"

using namespace std;
using namespace DirectX;
using namespace SimpleMath;

namespace Utility {
	
	float pythag(double x, double y, double z);
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
}

