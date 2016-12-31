#include "pch.h"
#include "EntityManager.h"

EntityManager::EntityManager(string worldName)
{

}

void EntityManager::RemoveEntity(unsigned int id)
{
	for (int i = 0; i < m_position.size(); i++) {
		if (m_position[i].ID == id) {
			m_position.erase(m_position.begin() + i,m_position.begin() + i);
		}
	} 
}

EntityManager::~EntityManager()
{
}
