#include "pch.h"
#include "TreeBranch.h"
namespace Tree {
	Branch::Branch(const Branch & other) : Start(other.Start), End(other.End),Branches(other.Branches)
	{
	}
	Branch::Branch(Vector3 start, Vector3 end) : Start(start), End(end)
	{

	}
	void Branch::Grow(int iterations)
	{
		if (iterations > 0) {
			float length = (Length() / GOLDEN_RATIO) * Utility::Deviation(0.4, 1.f);
			const float deviation = 1.f;
			Vector3 seed = End - Start;
			seed.Normalize();
			for (int i = 0; i < 2; i++) {
				Vector3 branch = Vector3(seed.x + Utility::Deviation(deviation), seed.y + Utility::Deviation(deviation), seed.z + Utility::Deviation(deviation));
				Branch nextBranch = Branch(End, End + branch * length);
				nextBranch.Grow(--iterations);
				Branches.push_back(nextBranch);
			}
		}
	}
	void Branch::GrowVertical(int iterations)
	{
		if (iterations) {
			float length = (Length() / GOLDEN_RATIO) * Utility::Deviation(0.4,1.f);
			auto rotation = XMMatrixRotationRollPitchYawFromVector(Vector3(Utility::randWithin(-PI / 18, PI / 18), 0.f, Utility::randWithin(-PI / 18, PI / 18)));
			Vector3 end = Vector3::Transform(Vector3::UnitY, rotation);
			Branch nextBranch = Branch(End, End + end * length);
			nextBranch.GrowVertical(--iterations);
			Branches.push_back(nextBranch);
		}
	}
	float Branch::Length()
	{
		return  Vector3::Distance(Start, End);
	}
	float Branch::TotalLength()
	{
		float length = Length();
		for (Branch & childBranch : Branches) {
			length += childBranch.TotalLength();
		}
		return length;
	}
}
