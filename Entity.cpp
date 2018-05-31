#include "pch.h"
#include "Entity.h"
#include "IEventManager.h"


Entity::Entity(const unsigned int & id, const unsigned long & mask, BaseEntityManager * entityManager) : m_id(id), m_mask(mask), m_entityManager(entityManager), m_removed(false)
{
}

EntityPtr Entity::Copy()
{
	return m_entityManager->Copy(this);
}

unsigned int Entity::ID()
{
	return m_id;
}

unsigned long Entity::GetMask()
{
	return m_mask;
}

bool Entity::IsRemoved()
{
	return m_removed;
}

bool Entity::HasComponents(const unsigned long & mask)
{
	return mask == (mask & m_mask);
}

unsigned long Entity::MissingComponents(const unsigned long & mask)
{
	// first xor yields difference, second removes extra components
	return (mask ^ m_mask) ^ m_mask;
}

vector<shared_ptr<Components::Component>> Entity::GetLoadedComponents()
{
	vector<shared_ptr<Components::Component>> components;
	for (auto & pair : m_components) {
		components.push_back(pair.second);
	}
	return components;
}

vector<shared_ptr<Components::Component>> Entity::GetComponents()
{
	vector<shared_ptr<Components::Component>> components;
	for (auto & mask : m_entityManager->ExtractMasks(m_mask)) {
		components.push_back(GetComponent<Components::Component>(mask));
	}
	return components;
}

void Entity::AddComponent(Components::Component * component)
{
	
	AddComponent(shared_ptr<Components::Component>(component));
	IEventManager::Invoke(Entity_ComponentAdded, m_id, m_entityManager->ComponentMask(component->GetName()));
}

void Entity::AddComponent(shared_ptr<Components::Component>& component)
{
	component->ID = m_id;
	unsigned long componentMask = m_entityManager->ComponentMask(component->GetName());
	
	m_mask |= componentMask;
	m_components.insert(std::pair<unsigned long, shared_ptr<Components::Component>>(componentMask, component));
	
}

void Entity::RemoveComponents(unsigned long mask)
{
	m_mask &= ~mask;
	for (auto & compMask : m_entityManager->ExtractMasks(mask)) {
		m_components.erase(compMask);
	}
}

void Entity::RemoveComponents()
{
	m_mask = 0;
	m_components.clear();
}

void Entity::RemoveEntity()
{
	
	m_removed = true;
}
