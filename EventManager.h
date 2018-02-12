#pragma once
#include "pch.h"
#include "EventHandler.h"
enum EventTypes {
	Action_Check,
	GUI_ShowHint,
	GUI_HideHint,
	Plant_GatherWood,
	Entity_ComponentAdded
};
template <typename ... Signature>
class EventManager
{
public:

	EventManager() {
		
	}
	static inline void RegisterHandler(EventTypes type, std::function<void(Signature...)> handler,unsigned int version) {
		if (m_handlers.find(type) == m_handlers.end()) {
			m_handlers.insert(std::make_pair(type, vector<GlobalEventHandler<Signature...>>()));
		}
		m_handlers[type].push_back(GlobalEventHandler<Signature...>(handler,version));
	}
	static inline void Invoke(EventTypes type, Signature... parameters, unsigned int version) {
		if (m_handlers.find(type) != m_handlers.end()) {
			for (auto & handler : m_handlers[type]) {
				if (handler.Version == version) {
					handler.Handler(parameters...);
				}
			}
		}
	}
	static std::map<EventTypes, vector<GlobalEventHandler<Signature...>>> m_handlers;
private:
	
};
template <typename ... Signature>
std::map<EventTypes, vector<GlobalEventHandler<Signature...>>> EventManager<Signature...>::m_handlers = std::map<EventTypes, vector<GlobalEventHandler<Signature...>>>();
