#include "pch.h"
#include "Action.h"

namespace Components {
	Action::Action(float radius) : Radius(radius)
	{

	}


	string Components::Action::GetName()
	{
		return "Action";
	}
}