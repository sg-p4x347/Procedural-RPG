#pragma once
#include "PositionNormalTextureVBO.h"
#include "TreeBranch.h"
#include "TopologyCruncher.h"
enum TreeType {
	SingleAxis,
	MultiAxis
};
class TreeGenerator
{
public:
	TreeGenerator();
	~TreeGenerator();
	Components::PositionNormalTextureTangentColorVBO Generate(TreeType type, int lod);
private:
	void GenerateBranches(TreeType  type, Tree::Branch & trunk,int iterations,XMMATRIX frame);
	void GenerateTopologyRecursive(Tree::Branch & branch, TopologyCruncher & tc, int lod);
	// Returns a translation as a function of t and tree type
	Vector3 BranchTranslation(TreeType & type, float t);
	// Returns a scaling vector as a function of t and tree type
	Vector3 BranchScale(TreeType & type, float t);
	// Returns a rotation vector as a funciton of an overall iteration, branch iteration and tree type
	Vector3 BranchRotation(TreeType & type, int n, int i);
	// Returns a terminal diameter constant as a function of starting diameter and tree type
	float TerminalDiameter(TreeType & type, float startDiameter);
	// Returns a constant as a function of tree type
	int BranchCount(TreeType & type);
};

