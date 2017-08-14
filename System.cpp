#include "pch.h"
#include "System.h"

System::System(shared_ptr<EntityManager> & entityManager, vector<string> & components, unsigned short updatePeriod)
{
	m_updatePeriod = updatePeriod;
	m_ticksLeft = updatePeriod;
}

void System::Tick(double & elapsed)
{
	if (--m_ticksLeft <= 0) {
		Update();
		m_ticksLeft = m_updatePeriod;
		m_elapsed = 0;
	} else {
		m_elapsed += elapsed;
	}
}
System::~System()
{
}
