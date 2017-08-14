#pragma once
// function signature typedefs
typedef void (*_f_)();
typedef void (*_f_entity)(unsigned int);

template <typename Signature>
class Event
{
public:
	//void Raise(Args arguments...);	// calls all listening functions

	// adds a listening function
	template<typename Signature>
	inline void Listen(Signature listener)
	{
		if (listener != nullptr) {
			m_listeners.push_back(listener);
		}
	}		
protected:
	std::vector<Signature> m_listeners;
};

