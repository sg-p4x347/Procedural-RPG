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
	shared_ptr<Components::Component> Children::Copy(shared_ptr<Components::Component> source)
	{
		return make_shared<GUI::Children>(*dynamic_pointer_cast<GUI::Children>(source));
	}
}
