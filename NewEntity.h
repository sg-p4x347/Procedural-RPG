#pragma once
#include "Event.h"
class NewEntity :
	public Event<_f_entity>
{
public:
	NewEntity();
	// calls all listening functions
	inline void Raise(unsigned int entity) {
		for (_f_entity & listener : m_listeners) {
			if (listener != nullptr) {
				listener(entity);
			}
		}
	}

};

