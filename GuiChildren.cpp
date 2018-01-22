#include "pch.h"
#include "GuiChildren.h"

namespace GUI {
	Children::Children()
	{
	}


	Children::Children(vector<unsigned int> children) : Children::Children()
	{
		Entities = children;
	}

	Children::~Children()
	{
	}

	string GUI::Children::GetName()
	{
		return "Children";
	}
}
