#include "pch.h"
#include "GuiPanel.h"

namespace GUI {
	Panel::Panel() : Panel::Panel("")
	{
	}

	Panel::Panel(string id) : Parent(0), ElementID(id), EventBubbling(false)
	{
	}



	Panel::~Panel()
	{
	}
	string Panel::GetName()
	{
		return "Panel";
	}
	shared_ptr<Components::Component> Panel::Copy(shared_ptr<Components::Component> source)
	{
		return make_shared<GUI::Panel>(*dynamic_pointer_cast<GUI::Panel>(source));
	}
}