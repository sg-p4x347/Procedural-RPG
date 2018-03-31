#pragma once
#include "QuadTree.h"
class EntityQuadTree :
	public QuadTree<unsigned int>
{
public:
	EntityQuadTree(Rectangle area, int minQuadWidth);
	vector<unsigned int> Find(Rectangle searchArea);
};

