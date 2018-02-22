#pragma once
#include "pch.h"
#include "EventHandler.h"
enum EventTypes {
	Game_Suspend,
	Game_Resume,

	Action_Check,
	Action_GatherWood,

	GUI_ShowHint,
	GUI_HideHint,
	GUI_OpenMenu,

	Sound_PlayEffect,
	Sound_PlayMusic,
	Sound_StopMusic
};
template <typename ... Signature>
class EventManager
{
public:

	EventManager() {
		
	}
	static inline void RegisterHandler(EventTypes type, std::function<void(Signature...)> handler,unsigned int version,bool isStatic) {
		if (m_handlers.find(type) == m_handlers.end()) {
			m_handlers.insert(std::make_pair(type, vector<GlobalEventHandler<Signature...>>()));
		}
		m_handlers[type].push_back(GlobalEventHandler<Signature...>(handler,version,isStatic));
	}
	static inline void Invoke(EventTypes type, Signature... parameters, unsigned int version) {
		if (m_handlers.find(type) != m_handlers.end()) {
			for (auto & handler : m_handlers[type]) {
				if (handler.IsStatic || handler.Version == version) {
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
