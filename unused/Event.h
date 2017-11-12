#pragma once
#include "BaseEvent.h"
template <typename Signature>
class Event :
	public BaseEvent
{
public:
	Event(string name);
	// calls all listening functions
	template<typename... Ts>
	inline void Raise(Ts... args) {
		for (Signature & listener : m_listeners) {
			if (listener != nullptr) {
				listener(args...);
			}
		}
	}
	// adds a listening function
	inline void AddListener(Signature listener) {
		if (listener != nullptr) {
			m_listeners.push_back(listener);
		}
	}
		
private:
	std::vector<Signature> m_listeners;

};

template<typename Signature>
inline Event<Signature>::Event(string name) : BaseEvent::BaseEvent(name)
{
}
