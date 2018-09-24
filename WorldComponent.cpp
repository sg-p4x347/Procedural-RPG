#include "pch.h"
#include "WorldComponent.h"

namespace world {

	WorldComponent::WorldComponent() : WorldComponent::WorldComponent(0)
	{
	}

	WorldComponent::WorldComponent(int id) : ID(id)
	{

	}


	WorldComponent::~WorldComponent()
	{
	}
}