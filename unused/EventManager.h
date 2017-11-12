#pragma once
#include "BaseEvent.h"
#include <map>
class EventManager
{
public:
	EventManager();
	~EventManager();
	template <typename EventType, typename... Ts>
	inline void Raise(string eventName, Ts... args) {
		std::shared_ptr<EventType> event = std::dynamic_pointer_cast<EventType>(m_events[eventName]);
		event->Raise(args...);
	}
	template <typename EventType, typename Signature>
	inline void AddListener(string eventName, Signature listener) {
		std::shared_ptr<EventType> event = std::dynamic_pointer_cast<EventType>(m_events[eventName]);
		event->AddListener(listener);
	}

	inline void Dummy() {
		string test = "";
	}
private:
	std::map<string, std::shared_ptr<BaseEvent>> m_events;
	void AddEvent(std::shared_ptr<BaseEvent> event);
};

// function signature typedefs
typedef void(EventManager::*_f_)();
typedef void(EventManager::*_f_entity)(unsigned int);

