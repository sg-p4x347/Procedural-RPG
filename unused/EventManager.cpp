#include "pch.h"
#include "EventManager.h"
#include "Event.h"

EventManager::EventManager()
{
	AddEvent(std::shared_ptr<BaseEvent>(new Event<_f_entity>("Update")));
	AddListener<Event<_f_>, _f_>("Update", &EventManager::Dummy);
	Raise<Event<_f_>>("Update");
}


EventManager::~EventManager()
{
}

void EventManager::AddEvent(std::shared_ptr<BaseEvent> event)
{
	m_events.insert(std::pair<string, std::shared_ptr<BaseEvent>>(event->Name(), event));
}
