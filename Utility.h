#pragma once
#include "pch.h"
#include "SimpleMath.h"

using namespace std;
using namespace DirectX;
using namespace SimpleMath;

namespace Utility {
	
	static const float sqrtTwo = 1.41421356237;
	float pythag(double x, double y, double z);
	//const double M_E = 2.71828182845904523536;
	int posToIndex(int x, int y, int width);
	int indexToX(int index, int width);
	int indexToY(int index, int width);
	void StringToWchar(string input,wchar_t * output);
	// Randomness
	float randWithin(float min, float max);
	int randWithin(int min, int max);
	float Deviation(float diameter, float offset = 0.f);
	bool Chance(float percent);
	int Probability(vector<float> probabilities);
	int sign(int integer);
	// Data tools
	template<typename T>
	void Concat(vector<T> & target, vector<T> & other) {
		if (target.size() == 0) {
			target = other;
		}
		else {
			target.insert(target.end(), other.begin(), other.end());
		}
	}
	void Normalize(std::vector<float> & set);
	template<typename T>
	inline void InitArray(T * array,const int & size,const T & value) {
		for (int i = 0; i < size; i++) {
			array[i] = value;
		}
	}
	void Normalize(float * set, int size);
	// Debug and logging tools
	void OutputException(string message);
	void OutputLine(string line);
	// Mathematical functions
	float Gaussian(float x, float a, float b, float c); // a controls amplituide; b controls x displacement; c controls width
	float Sigmoid(float x, float a, float b, float c); // a controls amplituide; b controls x displacement; c controls width
	float SigmoidDecay(float x, float width,float transitionPercent = 1.f); // sigmoid starting at (0.0,~1.0) decaying to (width,~0.0) at a rate given by transitionPercent
	float Inverse(float x, float a, float b, float c); // a controls amplituide; b controls x displacement; c controls width
	
	float BinomialCoefficient(int n, int k);
	template <typename T>
	T Clamp(T x, T min, T max);
	float Floor(float x, float precision);
	//----------------------------------------------------------------
	// Interpolation functions
	// 1 Dimension
	float Lerp(float start, float end, float t);
	// 2 Dimension
	float LinearInterpolate(float p[2], float x);
	float CubicInterpolate(float p[4], float x);
	// 2.5 Dimensions
	float BilinearInterpolate(float p[2][2], float x, float y);
	float BicubicInterpolation(float p[4][4], float x, float y);
	

	// Conversions
	float DegToRad(float degree);
	// Tests
	bool IsNumeric(const std::string& s);
	template<typename T>
	T Clamp(T x, T min, T max)
	{
		return std::min(max, std::max(min, x));
	}
}

