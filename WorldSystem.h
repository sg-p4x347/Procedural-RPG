#pragma once
#include "System.h"
#include "WorldDomain.h"
namespace world {
	class WorldSystem :
		public System
	{
	public:
		WorldSystem(world::WEM * entityManager, unsigned short updatePeriod);
	protected:
		world::WEM * EM;
	};
}

