#include "pch.h"
#include "GuiPanel.h"

namespace GUI {
	Panel::Panel() : Parent(0), ElementID("")
	{
	}

	Panel::Panel(string id) : Parent(0), ElementID(id)
	{
	}



	Panel::~Panel()
	{
	}
	string Panel::GetName()
	{
		return "Panel";
	}
}