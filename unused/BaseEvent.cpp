#include "pch.h"
#include "BaseEvent.h"


BaseEvent::BaseEvent(string name) : m_name(name)
{
}


BaseEvent::~BaseEvent()
{
}

string BaseEvent::Name()
{
	return m_name;
}
