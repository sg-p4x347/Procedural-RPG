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

shared_ptr<geometry::CMF> TreeGenerator::Generate(TreeType type)
{
	Branch trunk = Branch(Vector3::Zero, Vector3::UnitY,0.05f,TerminalDiameter(type,0.05f));
	//trunk.GrowVertical(4);
	float height = trunk.TotalLength();
	GenerateBranches(type, trunk,1,DirectX::XMMatrixIdentity());
	
	//tc.Tube(vector<Vector3>{Vector3(0, 0, 0), Vector3(10.f, 10.f, 10.f), Vector3(0, 15, 0)}, [](float t) {return t;}, 30, 10, PathType::BezierPath);
	shared_ptr<geometry::CMF> model = std::make_shared<geometry::CMF>("tree");
	shared_ptr<geometry::Material> material = std::make_shared<geometry::Material>();
	material->textures.push_back("wood.dds");
	material->pixelShader = "lambert.cso";
	material->ambientColor = Vector3::One;
	model->AddMaterial(material);
	for (int lod = 0; lod < 4; lod++) {
		TopologyCruncher tc;
		GenerateTopologyRecursive(trunk, tc, lod);

		model->AddLOD();
		auto mesh = std::make_shared<geometry::Mesh>();
		auto part = tc.CreateMeshPart();
		part.SetMaterial(material);
		mesh->AddPart(part);
		model->AddMesh(mesh,lod);
	}
	return model;
}
void TreeGenerator::GenerateBranches(TreeType type, Branch & trunk, int iteration, XMMATRIX frame)
{
	// Golden section in plants
	// https://www.sciencedirect.com/science/article/pii/S1002007108003419

	
		const int maxChildBranches = BranchCount(type);
		for (int n = 0; n < maxChildBranches; n++) {
			float t = (float)(n + 1) / (float)maxChildBranches;
			Vector3 rotation = BranchRotation(type,n,iteration);
			Vector3 translation = BranchTranslation(type, t);
			XMMATRIX transform = XMMatrixAffineTransformation(
				BranchScale(type, t), 
				Vector3::Zero, 
				SimpleMath::Quaternion::CreateFromYawPitchRoll(rotation.x, rotation.y, rotation.z), 
				translation
			);
			transform = XMMatrixMultiply(transform, frame);
			Vector3 start = Vector3::Transform(Vector3::Zero, transform);
			Vector3 end = Vector3::Transform(Vector3::UnitY, transform);

			float startDiameter = Utility::Lerp(trunk.StartDiameter, trunk.EndDiameter, translation.y);
			Branch child = Branch(
				start, 
				end,
				startDiameter,
				TerminalDiameter(type, startDiameter)
			);
			if (child.Length() > 0.05f && child.StartDiameter > 0.001f) {
				// Recurse over the child branches
				GenerateBranches(type, child, iteration + 1,transform);
				trunk.Branches.push_back(child);
			}
		}
	//Branch * trunkBranch = &trunk;
	//while (trunkBranch) {
	//	
	//	bool end = trunkBranch->Branches.size() == 0;
	//	const float startingYaw = Utility::randWithin(0.f, TWO_PI);
	//	float yaw = startingYaw;
	//	// Make several branches at this node
	//	vector<Branch> branches;
	//	while (yaw < startingYaw + TWO_PI) {
	//		float pitch = Utility::Deviation(PI / 6.f,-PI/8.f);
	//		auto rotation = XMMatrixRotationRollPitchYawFromVector(Vector3(pitch,yaw , 0.f));
	//		Vector3 start = trunkBranch->End;
	//		Vector3 end = Vector3::Transform(Vector3::UnitZ, rotation) * (trunkBranch->Length() / GOLDEN_RATIO);
	//		Branch branch = Branch(start, start + end);
	//		branch.Grow(3);
	//		trunkBranch->Branches.push_back(branch);
	//		yaw += Utility::randWithin(TWO_PI / 4.f, TWO_PI / 3.f);
	//	}
	//	trunkBranch = end ? nullptr : &trunkBranch->Branches[0];
	//}
}

void TreeGenerator::GenerateTopologyRecursive(Tree::Branch & branch, TopologyCruncher & tc, int lod)
{
	static const float scalingFactor = 7.f;
	static const float polygonWidth = 0.2f * (lod + 1);
	int radialDivisions = std::max(3, int((PI * branch.StartDiameter * scalingFactor) / polygonWidth));
	int longitudeDivisions = std::max(1, int((branch.Length() * scalingFactor) / polygonWidth));
	// Add variance between the branching points
	//static const float maxVariance = 0.01f;
	vector<Vector3> path{ branch.Start * scalingFactor };
	//Vector3 lastPoint = branch.Start;
	//for (auto & child : branch.Branches) {
	//	path.push_back(child.Start * scalingFactor);
	//	// Insert a new point between the last point and this one with a bit of variance
	//	path.push_back(Vector3::Lerp(lastPoint* scalingFactor, child.Start * scalingFactor, 0.5f) + ProUtil::RandomVec3(maxVariance));

	//	lastPoint = child.Start;
	//}
	path.push_back(branch.End * scalingFactor);
	// Generate the topology
	tc.Tube(path, [&branch](float & t) {
		return Utility::Lerp(branch.StartDiameter * scalingFactor, branch.EndDiameter * scalingFactor, t);
	}, longitudeDivisions, radialDivisions, PathType::BezierPath);
	// recurse over children
	for (auto & child : branch.Branches) {
		if (child.Length() > 0.05 * lod) {
			GenerateTopologyRecursive(child, tc,lod);
		}
	}

	//points.push_back(branch.Start);
	//Branch * longest = nullptr;
	//for (Branch & nextBranch : branch.Branches) {
	//	if (!longest) {
	//		longest = &nextBranch;
	//	}
	//	else if (nextBranch.Length() > longest->Length()) {
	//		longest = &nextBranch;
	//	}
	//}
	//if (longest) {
	//	for (Branch & nextBranch : branch.Branches) {
	//		if (&nextBranch != longest) {
	//			// Start a new tube
	//			GenerateTopologyRecursive(nextBranch, tc, vector<Vector3>(),nextBranch.Length() / 10.f);
	//		}
	//		else {
	//			// Longest branch continues the chain
	//			GenerateTopologyRecursive(*longest, tc, points,diameter);
	//		}
	//	}
	//}
	//else {
	//	// The end of this branch
	//	points.push_back(branch.End);
	//	// make the tube
	//	tc.Tube(points, [diameter](float & t) {
	//		return Utility::Lerp(diameter, 0.f, t);
	//	},10,10,PathType::BezierPath);
	//}

}

Vector3 TreeGenerator::BranchTranslation(TreeType & type, float t)
{
	static const float offset = 0.15f;
	switch (type) {
	case TreeType::SingleAxis:
		return (Vector3::UnitY * t * (0.9f - offset) + Vector3::UnitY * offset);
	case TreeType::MultiAxis:
		return Vector3::UnitY;
	}
	
}

Vector3 TreeGenerator::BranchScale(TreeType & type, float t)
{
	switch (type) {
	case TreeType::SingleAxis:
		return Vector3::One * (-pow(t-0.5f,4) - (0.1f * t) + 0.5f);
	case TreeType::MultiAxis:
		return Vector3::One * 0.62f;
	}
	
	//return Vector3::One * (0.5f * sin(PI * t) + 0.2);
	//return Vector3::One * (-1 * (std::pow(t - (1-std::sqrt(a)), 2)) + a);
	//return Vector3::One * (-0.1f*std::pow(t - 0.1f, 2)- std::pow(t - 0.1f, 6) + 0.9f);
}

Vector3 TreeGenerator::BranchRotation(TreeType & type, int n, int i)
{
	// 137.5 degrees = 2.3... rad
	// 34.4 degrees = 0.6 rad
	// 55.6 degrees = 0.97 rad
	switch (type) {
	case TreeType::SingleAxis:
		return Vector3(2.3998277f * n, Utility::Deviation(0.f,.6f), 0.f);
	case TreeType::MultiAxis:
		return Vector3(2.3998277f * n, Utility::Deviation(0.f, 0.97f), 0.f);
	}
	
}

float TreeGenerator::TerminalDiameter(TreeType & type, float startDiameter)
{
	switch (type) {
	case TreeType::SingleAxis:
		return 0.001f;
	case TreeType::MultiAxis:
		return 0.62f * startDiameter;
	}
}

int TreeGenerator::BranchCount(TreeType & type)
{
	switch (type) {
	case TreeType::SingleAxis:
		return 6;
	case TreeType::MultiAxis:
		return 4;
	}
}


