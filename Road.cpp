#include "pch.h"
#include "Road.h"

namespace Components {
	Road::Road(const unsigned int & id) : Component::Component(id)
	{
	}


	Road::~Road()
	{
	}
	string Road::GetName()
	{
		return "Road";
	}
}