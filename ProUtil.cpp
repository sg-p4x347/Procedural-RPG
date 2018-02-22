#include "pch.h"
#include "ProUtil.h"
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Rectangle;
namespace ProUtil {
	
	//----------------------------------------------------------------
	// Methods

	// Returns a golden rectangle with the narrow side as the width
	Rectangle ProUtil::GoldenRect(double maxArea) {
		int A = std::floor(std::sqrt(maxArea / GR));
		int B = maxArea / (double)A;
		return Rectangle(0, 0, A, B);
	}
	Rectangle ProUtil::RectWithin(Rectangle A, Rectangle B) {
		long width = RandWithin(std::min(A.width, B.width), std::max(A.width, B.width));
		long height = RandWithin(std::min(A.height, B.height), std::max(A.height, B.height));
		return Rectangle(0, 0, width, height);
	}
	Vector2 ProUtil::PointWithin(Rectangle A)
	{
		return Vector2(RandWithin(A.x,A.x + A.width),RandWithin(A.y,A.y + A.height));
	}
	float ProUtil::RandWithin(float min, float max) {
		return (float(rand()) / float(RAND_MAX))*(max - min) + min;
	}
	int ProUtil::RandWithin(int min, int max) {
		return rand() % (max - min) + min;
	}
	int ToRandom(string input)
	{
		int result = 0;
		for (int i = 0; i < input.length(); i++) {
			result += (int)input[i];
		}
		return result;
	}
}