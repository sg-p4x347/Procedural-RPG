#pragma once
#include "EntityTypedefs.h"
namespace world {
	class WorldComponent
	{
	public:
		WorldComponent();
		WorldComponent(int id);
		~WorldComponent();
		EntityID ID;
	};
}

