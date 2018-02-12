#include "pch.h"
#include "IEventManager.h"

unsigned int IEventManager::m_currentVersion = 0;

void IEventManager::NewVersion()
{
	m_currentVersion++;
}

unsigned int IEventManager::GetCurrentVersion()
{
	return m_currentVersion;
}
