#include "pch.h"
#include "WorldSystem.h"

WorldSystem::WorldSystem(shared_ptr<EntityManager> entityManager, vector<string>& components, unsigned short updatePeriod) : System::System(updatePeriod), EM(entityManager)
{
	m_componentMask = EM->ComponentMask(components);
}
