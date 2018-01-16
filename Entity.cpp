#include "pch.h"
#include "Entity.h"

Entity::Entity(unsigned int id, unsigned long componentMask) : m_id(id), m_cachedMask(0), m_componentMask(componentMask)
{
}

unsigned int Entity::ID()
{
	return m_id;
}

unsigned long Entity::ComponentMask()
{
	return m_componentMask;
}

unsigned long Entity::CachedMask()
{
	return m_cachedMask;
}

bool Entity::HasComponents(unsigned long componentMask)
{
	return componentMask == (componentMask & m_componentMask);
}

unsigned long Entity::MissingComponents(unsigned long & componentMask)
{
	// first xor yields difference, second removes extra components
	return (componentMask ^ m_componentMask) ^ m_componentMask;
}

void Entity::AddComponent(unsigned long componentMask, shared_ptr<Components::Component> component)
{
	component->ID = m_id;
	m_cachedMask |= componentMask;
	m_componentMask |= componentMask;
	m_components.insert(std::pair < unsigned long, shared_ptr<Components::Component>>(componentMask, component));
}

void Entity::Save(Filesystem::path directory)
{
	for (std::map<unsigned long, shared_ptr<Components::Component>>::iterator it = m_components.begin(); it != m_components.end(); ++it) {
		it->second->Save(directory);
	}
}

map<unsigned long, shared_ptr<Components::Component>> & Entity::Components()
{
	return m_components;
}

//shared_ptr<Components::Component> Entity::GetComponent(unsigned long & mask)
//{
//	// search the cache
//	map<unsigned long, shared_ptr<Components::Component>>::iterator it = m_components.find(mask);
//	if (it != m_components.end()) {
//		return it->second;
//	}
//	// load the component
//	return EM->GetComponent(mask, EntityPtr(this));
//}

//void Entity::Import(JsonParser & jp)
//{
//	m_id = (unsigned int)jp["id"];
//	m_componentMask = (unsigned long)jp["cm"];
//	JsonParser components = jp["comps"];
//	for (string type : components.GetKeys()) {
//
//	}
//}
//
//JsonParser Entity::Export()
//{
//	JsonParser obj;
//	obj.Set("id", ID());
//	obj.Set("cm", ComponentMask());
//	
//	return obj;
//}
