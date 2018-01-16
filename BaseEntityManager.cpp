#include "pch.h"
#include "BaseEntityManager.h"


BaseEntityManager::BaseEntityManager()
{
	m_nextID = 1;
}


BaseEntityManager::~BaseEntityManager()
{
}

unsigned long BaseEntityManager::ComponentMask(vector<string> components)
{
	unsigned long mask = 0;
	for (string & componentName : components) {
		mask |= ComponentMask(componentName);
	}
	return mask;
}

unsigned long BaseEntityManager::ComponentMask(string component)
{
	return m_masks[component];
}

unsigned long BaseEntityManager::ComponentMaskOf(const unsigned int & id)
{
	EntityPtr entity;
	if (Find(id, entity)) {
		return entity->ComponentMask();
	}
	return 0;
}

EntityPtr BaseEntityManager::NewEntity()
{

	auto entity = std::make_shared<Entity>(m_nextID++,0);
	m_entities.insert(std::pair<unsigned int, EntityPtr>(entity->ID(), entity));
	return entity;
}

vector<EntityPtr> BaseEntityManager::FindEntities(unsigned long componentMask)
{
	// search cache
	vector<EntityPtr> entities;
	for (unordered_map<unsigned int, EntityPtr>::iterator it = m_entities.begin(); it != m_entities.end(); it++) {
		if (it->second->HasComponents(componentMask)) entities.push_back(it->second);
	}
	return entities;
}

vector<EntityPtr> BaseEntityManager::FindEntities(vector<unsigned int> ids)
{
	vector<EntityPtr> entities;
	for (unsigned int & id : ids) {
		EntityPtr entity;
		if (Find(id, entity)) {
			entities.push_back(entity);
		}
	}
	return entities;
}

bool BaseEntityManager::Find(const unsigned int & id, EntityPtr& entity)
{
	// search cache
	if (m_entities.find(id) != m_entities.end()) {
		entity = m_entities[id];
		return true;
	}
	else {
		entity = nullptr;
		return false;
	}
}
void BaseEntityManager::AddComponentVector(string name)
{
	static unsigned int index = 0;

	unsigned long mask = std::pow(2, index++);
	m_masks.insert(std::pair<string, unsigned long>(name, mask));
	m_names.insert(std::pair<unsigned long, string>(mask, name));
}

shared_ptr<Components::Component> BaseEntityManager::GetComponent(unsigned long & mask, EntityPtr entity)
{
	// search the cache
	map<unsigned long, shared_ptr<Components::Component>>::iterator it = entity->Components().find(mask);
	if (!entity->Components().empty() && it != entity->Components().end()) {
		return it->second;
	}
	// load from file
	shared_ptr<Components::Component> component = InstantiateComponent(mask, entity);

	if (component) {
		if(Filesystem::is_directory(m_directory)) component->Load(m_directory, entity->ID());

		entity->AddComponent(mask, component);
		return component;
	}
	// not found
	else {
		return nullptr;
	}
}
