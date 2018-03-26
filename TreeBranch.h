#pragma once
namespace Tree {
	class Branch
	{
	public:
		Branch(const Branch & other);
		Branch(Vector3 start, Vector3 end,float startDiameter, float endDiameter);
		//----------------------------------------------------------------
		// Data
		Vector3 Start;
		Vector3 End;
		float StartDiameter;
		float EndDiameter;
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

