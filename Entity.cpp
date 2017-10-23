#include "pch.h"
#include "Entity.h"

Entity::Entity(unsigned int id) : m_id(id)
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

bool Entity::HasComponents(unsigned long & componentMask)
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
	m_componentMask |= componentMask;
	m_components.insert(std::pair < string, shared_ptr<Components::Component>>(component->GetName(), component));
}

map<string, shared_ptr<Components::Component>>& Entity::GetComponents()
{
	return m_components;
}

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
