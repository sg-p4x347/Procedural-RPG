#pragma once
#include "pch.h"
#include "EventHandler.h"
enum EventTypes {
	Entity_ComponentAdded,
	Entity_Deleted,

	Game_Suspend,
	Game_Resume,

	Action_Check,

	GUI_ShowHint,
	GUI_HideHint,
	GUI_OpenMenu,
	GUI_CloseMenu,

	Sound_PlayEffect,
	Sound_PlayMusic,
	Sound_StopMusic,

	Item_OpenInventory,
	Item_CloseInventory,
	Item_Transfer,

	Resource_Aquire,

	Player_SetInteractionState,
	Player_OpenInventory,

	Movement_PlayerMoved
};
template <typename KeyType, typename ... Signature>
class EventManager
{
public:

	EventManager() {
		
	}
	static inline void RegisterHandler(KeyType type, std::function<void(Signature...)> handler,unsigned int version,bool isStatic) {
		if (m_handlers.find(type) == m_handlers.end()) {
			m_handlers.insert(std::make_pair(type, vector<GlobalEventHandler<Signature...>>()));
		}
		m_handlers[type].push_back(GlobalEventHandler<Signature...>(handler,version,isStatic));
	}
	static inline void Invoke(KeyType type, Signature... parameters, unsigned int version) {
		if (m_handlers.find(type) != m_handlers.end()) {
			for (auto & handler : m_handlers[type]) {
				if (handler.IsStatic || handler.Version == version) {
					handler.Handler(parameters...);
				}
			}
		}
	}
	static std::map<KeyType, vector<GlobalEventHandler<Signature...>>> m_handlers;
private:
	
};
template <typename KeyType, typename ... Signature>
std::map<KeyType, vector<GlobalEventHandler<Signature...>>> EventManager<KeyType,Signature...>::m_handlers = std::map<KeyType, vector<GlobalEventHandler<Signature...>>>();
