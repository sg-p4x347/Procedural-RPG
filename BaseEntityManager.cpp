#include "pch.h"
#include "BaseEntityManager.h"
#include "Entity.h"
#include <bitset>
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

vector<unsigned long> BaseEntityManager::ExtractMasks(unsigned long mask)
{
	vector<unsigned long> masks;
	std::bitset<m_maskSize> bitmask(mask);
	for (int i = 0; i < m_maskSize; i++) {
		if (bitmask[i])
			masks.push_back(std::pow(2, i));
	}
	return masks;
}

unsigned long BaseEntityManager::ComponentMaskOf(const unsigned int & id)
{
	EntityPtr entity;
	if (Find(id, entity)) {
		return entity->GetMask();
	}
	return 0;
}

string BaseEntityManager::NameOf(const unsigned long & mask)
{
	if (m_names.find(mask) != m_names.end()) {
		return m_names[mask];
	}
	return "";
}

EntityPtr BaseEntityManager::NewEntity()
{
	std::lock_guard<shared_mutex> lock(m_mutex);
	std::shared_ptr<Entity> entity;
	if (m_removedIDs.empty()) {
		//----------------------------------------------------------------
		// new ID
		entity = std::shared_ptr<Entity>(new Entity(m_nextID++, 0, this));
	}
	else {
		//----------------------------------------------------------------
		// Recycled ID
		entity = std::shared_ptr<Entity>(new Entity(m_removedIDs.front(), 0, this));
		m_removedIDs.pop();
		
	}
	if (m_entities.find(entity->ID()) != m_entities.end()) {
		m_entities[entity->ID()] = entity;
	}
	else {
		m_entities.insert(std::pair<unsigned int, EntityPtr>(entity->ID(), entity));
	}
	return entity;
}

void BaseEntityManager::DeleteEntity(EntityPtr & entity)
{
	m_removedIDs.push(entity->ID());
	// Remove all components from the entity
	entity->RemoveComponents();
	entity->RemoveEntity();
}

vector<EntityPtr> BaseEntityManager::FindEntities(string compName)
{
	return FindEntities(ComponentMask(compName));
}

vector<EntityPtr> BaseEntityManager::FindEntities(unsigned long componentMask)
{
	std::lock_guard<shared_mutex> lock(m_mutex);
	// search cache
	vector<EntityPtr> entities;
	for (unordered_map<unsigned int, EntityPtr>::iterator it = m_entities.begin(); it != m_entities.end(); it++) {
		if (it->second->HasComponents(componentMask)) entities.push_back(it->second);
	}
	// other means
	vector<EntityPtr> uncached = LoadEntities(componentMask);
	// cache the new entities
	for (auto & entity : uncached) {
		m_entities.insert(std::pair<unsigned int, EntityPtr>(entity->ID(), entity));
	}
	// return a union of cached and previously un-cached entities
	entities.insert(entities.end(),uncached.begin(), uncached.end());
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
void BaseEntityManager::RegisterComponent(std::function<Components::Component*()>&& instantiate)
{
	auto prototype = instantiate();
	string name = prototype->GetName();
	BaseEntityManager::RegisterComponent(name);
	m_prototypes.insert(std::pair<unsigned long, std::function<Components::Component*()> >(ComponentMask(name), instantiate));
	delete prototype;
}

void BaseEntityManager::RegisterDelegate(std::function<Components::Component*(string delegateName)>&& instantiate, vector<string> delegateNames)
{
	for (string & delegateName : delegateNames) {
		RegisterComponent([instantiate, delegateName] {return instantiate(delegateName);});
	}
}
void BaseEntityManager::RegisterComponent(string name)
{

	unsigned long mask = std::pow(2, m_masks.size());
	m_masks.insert(std::pair<string, unsigned long>(name, mask));
	m_names.insert(std::pair<unsigned long, string>(mask, name));
}

shared_ptr<Components::Component> BaseEntityManager::LoadComponent(unsigned long & mask, Entity * entity)
{
	return nullptr;
}

shared_ptr<Components::Component> BaseEntityManager::GetPrototype(unsigned long & mask)
{
	return shared_ptr<Components::Component>(m_prototypes[mask]());
}

shared_ptr<Components::Component> BaseEntityManager::GetComponent(unsigned long & mask, Entity * entity)
{
	//// search the cache
	//map<unsigned long, shared_ptr<Components::Component>>::iterator it = entity->Components().find(mask);
	//if (!entity->Components().empty() && it != entity->Components().end()) {
	//	return it->second;
	//}
	// other means
	std::lock_guard<shared_mutex> lock(m_mutex);
	shared_ptr<Components::Component> component = LoadComponent(mask, entity);
	// cache the component
	if (component) {
		entity->AddComponent(component);
	}
	// return the freshly cached component
	return component;
}

vector<EntityPtr> BaseEntityManager::LoadEntities(unsigned long & mask)
{
	return vector<EntityPtr>();
}

void BaseEntityManager::SetNextID(unsigned int & id)
{
	m_nextID = id;
}

unsigned int BaseEntityManager::GetNextID()
{
	return m_nextID;
}
