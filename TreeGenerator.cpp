#include "pch.h"
#include "TreeGenerator.h"
#include "TreeBranch.h"
#include "TopologyCruncher.h"

using namespace Tree;
TreeGenerator::TreeGenerator()
{

}


TreeGenerator::~TreeGenerator()
{
}

Components::PositionNormalTextureVBO TreeGenerator::Generate()
{
	Branch trunk = Branch(Vector3::Zero, Vector3(0.f, Utility::randWithin(2.f, 4.f), 0.f));
	trunk.GrowVertical(4);
	float height = trunk.TotalLength();
	//GenerateBranches(trunk);

	TopologyCruncher tc;
	//tc.Tube(vector<Vector3>{Vector3(0, 0, 0), Vector3(10.f, 10.f, 10.f), Vector3(0, 15, 0)}, [](float t) {return t;}, 30, 10, PathType::BezierPath);
	GenerateTopologyRecursive(trunk, tc, vector<Vector3>(),height / 30.f);
	return tc.CreateVBO();
}
void TreeGenerator::GenerateBranches(Branch & trunk)
{
	Branch * trunkBranch = &trunk;
	while (trunkBranch) {
		
		bool end = trunkBranch->Branches.size() == 0;
		const float startingYaw = Utility::randWithin(0.f, TWO_PI);
		float yaw = startingYaw;
		// Make several branches at this node
		vector<Branch> branches;
		while (yaw < startingYaw + TWO_PI) {
			float pitch = Utility::Deviation(PI / 6.f,-PI/8.f);
			auto rotation = XMMatrixRotationRollPitchYawFromVector(Vector3(pitch,yaw , 0.f));
			Vector3 start = trunkBranch->End;
			Vector3 end = Vector3::Transform(Vector3::UnitZ, rotation) * (trunkBranch->Length() / GOLDEN_RATIO);
			Branch branch = Branch(start, start + end);
			branch.Grow(3);
			trunkBranch->Branches.push_back(branch);
			yaw += Utility::randWithin(TWO_PI / 4.f, TWO_PI / 3.f);
		}
		trunkBranch = end ? nullptr : &trunkBranch->Branches[0];
	}
}

void TreeGenerator::GenerateTopologyRecursive(Tree::Branch & branch, TopologyCruncher & tc, vector<Vector3> points, float diameter)
{
	points.push_back(branch.Start);
	Branch * longest = nullptr;
	for (Branch & nextBranch : branch.Branches) {
		if (!longest) {
			longest = &nextBranch;
		}
		else if (nextBranch.Length() > longest->Length()) {
			longest = &nextBranch;
		}
	}
	if (longest) {
		for (Branch & nextBranch : branch.Branches) {
			if (&nextBranch != longest) {
				// Start a new tube
				GenerateTopologyRecursive(nextBranch, tc, vector<Vector3>(),nextBranch.Length() / 10.f);
			}
			else {
				// Longest branch continues the chain
				GenerateTopologyRecursive(*longest, tc, points,diameter);
			}
		}
	}
	else {
		// The end of this branch
		points.push_back(branch.End);
		// make the tube
		tc.Tube(points, [diameter](float & t) {
			return Utility::Lerp(diameter, 0.f, t);
		},10,10,PathType::BezierPath);
	}

}


