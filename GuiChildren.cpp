#include "pch.h"
#include "GuiChildren.h"

namespace GUI {
	Children::Children(const unsigned int & id) : Components::Component(id)
	{
	}

	Children::Children() : Children::Children(0)
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
