#pragma once
#include "pch.h"
enum EventTypes {
	Action_Check
};
template <typename ... Signature>
class EventManager
{
public:

	EventManager() {
		
	}
	static inline void RegisterHandler(EventTypes type, std::function<void(Signature...)> handler) {
		if (m_handlers.find(type) == m_handlers.end()) {
			m_handlers.insert(std::make_pair(type, vector<std::function<void(Signature...)>>()));
		}
		m_handlers[type].push_back(handler);
	}
	static inline void Invoke(EventTypes type, Signature... parameters) {
		if (m_handlers.find(type) != m_handlers.end()) {
			for (auto & handler : m_handlers[type]) {
				handler(parameters...);
			}
		}
	}
	static std::map<EventTypes, vector<std::function<void(Signature...)>>> m_handlers;
private:
	
};
template <typename ... Signature>
std::map<EventTypes, vector<std::function<void(Signature...)>>> EventManager<Signature...>::m_handlers = std::map<EventTypes, vector<std::function<void(Signature...)>>>();
