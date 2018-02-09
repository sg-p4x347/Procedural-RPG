#pragma once
#include "EventManager.h"
class IEventManager
{
public:
	template <typename ... Signature>
	static inline void RegisterHandler(EventTypes type, std::function<void(Signature...)> handler) {
		EventManager<Signature...>::RegisterHandler(type, handler);
	}
	template <typename ... Signature>
	static inline void Invoke(EventTypes type, Signature... parameters) {
		EventManager<Signature...>::Invoke(type, parameters...);
	}
};

