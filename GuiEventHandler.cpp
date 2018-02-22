#include "pch.h"
#include "GuiEventHandler.h"


namespace GUI {

	EventHandler::EventHandler(string type) : Components::Delegate(type)
	{
	}

	EventHandler::EventHandler(string type, std::function<void(Event evt)>&& callback) : EventHandler::EventHandler(type)
	{
		Callback = std::move(callback);
	}


	EventHandler::~EventHandler()
	{
	}

	string EventHandler::GetDiscreteName()
	{
		return "EventHandler";
	}
}
