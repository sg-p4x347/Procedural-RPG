#pragma once
#include "PositionNormalTextureVBO.h"
#include "TreeBranch.h"
#include "TopologyCruncher.h"
class TreeGenerator
{
public:
	TreeGenerator();
	~TreeGenerator();
	Components::PositionNormalTextureVBO Generate();
private:
	void GenerateBranches(Tree::Branch & trunk);
	void GenerateTopologyRecursive(Tree::Branch & branch, TopologyCruncher & tc,vector<Vector3> points,float diameter);
};

