#include "pch.h"
#include "WorldSystem.h"

WorldSystem::WorldSystem(unique_ptr<WorldEntityManager> & entityManager, vector<string>& components, unsigned short updatePeriod) : System::System(updatePeriod), EM(entityManager)
{
	m_componentMask = EM->ComponentMask(components);
}
