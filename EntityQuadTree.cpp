#include "pch.h"
#include "EntityQuadTree.h"


EntityQuadTree::EntityQuadTree(Rectangle area, int minQuadWidth) : QuadTree<unsigned int>::QuadTree(area)
{
	Grow(minQuadWidth);
}

vector<unsigned int> EntityQuadTree::Find(Rectangle searchArea)
{
	vector<unsigned int> results;
	QuadTree<unsigned int>::Find(searchArea, results);
	return results;
}

