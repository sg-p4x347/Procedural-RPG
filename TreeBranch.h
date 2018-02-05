#pragma once
namespace Tree {
	class Branch
	{
	public:
		Branch(const Branch & other);
		Branch(Vector3 start, Vector3 end);
		//----------------------------------------------------------------
		// Data
		Vector3 Start;
		Vector3 End;
		vector<Branch> Branches;
		//----------------------------------------------------------------
		// Modifiers
		void Grow(int iterations);
		void GrowVertical(int iterations);
		//----------------------------------------------------------------
		// Calculated Properties
		float Length();
		float TotalLength();
	};
}

