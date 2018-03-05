#include "pch.h"
#include "System.h"

System::System(
	unsigned short updatePeriod
) : m_updatePeriod(updatePeriod), m_ticksLeft(updatePeriod), m_elapsed(0.0), m_halted(false)
{
}

void System::Tick(double & elapsed)
{
	if (!m_halted && m_updatePeriod != 0) {
		m_elapsed += elapsed;
		if (--m_ticksLeft <= 0) {
			Update(m_elapsed);
			m_ticksLeft = m_updatePeriod;
			m_elapsed = 0;
		}
	}
}
void System::Initialize()
{
	//SyncEntities();
}

void System::Save()
{
}

void System::SyncEntities()
{
}

void System::Halt()
{
	m_halted = true;
}

void System::Run()
{
	m_halted = false;
}
