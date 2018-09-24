#include "pch.h"
#include "WorldSystem.h"

namespace world {
	WorldSystem::WorldSystem(WEM * entityManager, unsigned short updatePeriod) : System::System(updatePeriod), EM(entityManager)
	{
	}
}
