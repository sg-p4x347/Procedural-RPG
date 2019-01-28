#include "pch.h"
#include "Utility.h"
#include <cctype>
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
		if (percent > 0.f) {
			return randWithin(0.f, 1.f) <= percent;
		}
		else {
			return false;
		}
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
		for (i = 0, j = (int)polygon.size() - 1; i < (int)polygon.size(); j = i++) {
			if (((polygon[i].y>test.y) != (polygon[j].y>test.y)) &&
				(test.x < (polygon[j].x - polygon[i].x) * (test.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x))
				c = !c;
		}
		return c;
	}
	float Deviation(float diameter, float offset) {
		return randWithin(-diameter * 0.5f + offset, diameter * 0.5f + offset);
	}
	void Normalize(float * set, int size)
	{
		float sum = 0.f;
		for (int i = 0; i < size; i++) {
			sum += std::abs(set[i]);
		}
		if (sum != 0.f) {
			for (int i = 0; i < size; i++) {
				set[i] /= sum;
			}
		}
	}
	void OutputException(string message)
	{
		static string border = "!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!\n";
		OutputDebugStringA((border + message + '\n' + border).c_str());
	}
	void OutputLine(string line)
	{
		OutputDebugStringA((line + '\n').c_str());
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

	float BinomialCoefficient(int n, int k)
	{
		float result = 1.f;
		for (int i = 1; i <= k; i++) {
			result *= (float)(n + 1 - i) / (float)i;
		}
		return result;
	}

	float Clamp(float x, float min, float max)
	{
		return std::min(max, std::max(min, x));
	}

	float Floor(float x, float precision)
	{
		return std::floor((1.f / precision) * x) * precision;
	}

	float SigmoidDecay(float x, float width, float transitionPercent)
	{
		return Sigmoid(x, 1.f, 0.5 * width, -10 / (width * transitionPercent));
	}

	float Lerp(float start, float end, float t)
	{
		return start * (1 - t) + end * t;;
	}

	float LinearInterpolate(float p[2], float x)
	{
		return Lerp(p[0], p[1], x);
	}

	void Normalize(std::vector<float> & set)
	{
		float sum = 0.f;
		for (float value : set) {
			sum += std::abs(value);
		}
		if (sum != 0.f) {
			for (int i = 0; i < set.size(); i++) {
				set[i] /= sum;
			}
		}
	}

	float BilinearInterpolate(float p[2][2], float x, float y)
	{
		/*float arr[2];
		arr[0] = LinearInterpolate(p[0], y);
		arr[1] = LinearInterpolate(p[1], y);
		return LinearInterpolate(arr, x);*/
		return p[0][0] * (1 - x)*(1 - y) + p[1][0] * x*(1 - y) + p[0][1] * (1 - x)*y + p[1][1] * x*y;
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
	
	float DegToRad(float degree)
	{
		static const float PIdiv180 = PI / 180.f;
		return PIdiv180 * degree;
	}
	bool IsNumeric(const std::string & s)
	{
		std::string::const_iterator it = s.begin();
		while (it != s.end() && std::isdigit(*it)) ++it;
		return !s.empty() && it == s.end();
	}
	float CubicInterpolate(float p[4], float x) {
		return p[1] + 0.5 * x*(p[2] - p[0] + x*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x*(3.0*(p[1] - p[2]) + p[3] - p[0])));
	}
}