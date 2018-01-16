#include "pch.h"
#include "GuiEventHandler.h"

namespace GUI {

	EventHandler::EventHandler(const unsigned int & id, string type) : Components::Component(id)
	{
		Type = type;
	}

	EventHandler::EventHandler(string type, std::function<void()>&& callback) : EventHandler::EventHandler(0,type)
	{
		Callback = std::move(callback);
	}


	EventHandler::~EventHandler()
	{
	}

	string GUI::EventHandler::GetName()
	{
		return "EventHandler_" + Type;
	}
}
