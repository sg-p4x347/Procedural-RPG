#include "pch.h"
#include "Entity.h"
#include "IEventManager.h"

Entity::Entity(const unsigned int & id, const unsigned long & mask, BaseEntityManager * entityManager) : m_id(id), m_mask(mask), m_entityManager(entityManager)
{
}

unsigned int Entity::ID()
{
	return m_id;
}

unsigned long Entity::GetMask()
{
	return m_mask;
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

vector<shared_ptr<Components::Component>> Entity::GetComponents()
{
	vector<shared_ptr<Components::Component>> components;
	for (auto & pair : m_components) {
		components.push_back(pair.second);
	}
	return components;
}

void Entity::AddComponent(Components::Component * component)
{
	
	AddComponent(shared_ptr<Components::Component>(component));
}

void Entity::AddComponent(shared_ptr<Components::Component>& component)
{
	component->ID = m_id;
	unsigned long componentMask = m_entityManager->ComponentMask(component->GetName());
	
	m_mask |= componentMask;
	m_components.insert(std::pair < unsigned long, shared_ptr<Components::Component>>(componentMask, component));
	IEventManager::Invoke(Entity_ComponentAdded, m_id, componentMask);
}
