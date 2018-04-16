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
	shared_ptr<Components::Component> EventHandler::Copy(shared_ptr<Components::Component> source)
	{
		return make_shared<GUI::EventHandler>(*dynamic_pointer_cast<GUI::EventHandler>(source));
	}
}
