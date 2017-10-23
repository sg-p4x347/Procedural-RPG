#include "pch.h"
#include "System.h"

System::System(
	shared_ptr<EntityManager> & entityManager,
	vector<string> & components,
	unsigned short updatePeriod
) : EM(entityManager), m_updatePeriod(updatePeriod), m_ticksLeft(updatePeriod)
{
	m_componentMask = EM->ComponentMask(components);
}

void System::Tick(double & elapsed)
{
	if (--m_ticksLeft <= 0) {
		SyncEntities();
		Update();
		m_ticksLeft = m_updatePeriod;
		m_elapsed = 0;
	} else {
		m_elapsed += elapsed;
	}
}
void System::Initialize()
{
}
