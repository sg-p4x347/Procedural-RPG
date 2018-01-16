#include "pch.h"
#include "GuiPanel.h"

namespace GUI {
	Panel::Panel(const unsigned int & id) : Components::Component(id)
	{
	}

	Panel::Panel() : Panel::Panel(0)
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