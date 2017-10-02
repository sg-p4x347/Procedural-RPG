#include "pch.h"
#include "EntityManager.h"
#include "JsonParser.h"
#include <bitset>
// Concrete Component types
#include "Position.h"
#include "Movement.h"
#include "Input.h"
#include "Flag.h"
#include "Terrain.h"

shared_ptr<Component::Component> EntityManager::GetComponent(const unsigned int & id, string componentName)
{
	return GetComponent(id, m_masks[componentName]);
}

shared_ptr<Component::Component> EntityManager::GetComponent(const unsigned int & id, unsigned long componentMask)
{
	shared_ptr<Component::Component> component = m_prototypes[componentMask]->GetComponent(id);
	if (!component) component = m_prototypes[componentMask]->Load(m_directory, id);
	return component;
}

map<string, shared_ptr<Component::Component>> EntityManager::GetComponents(const unsigned int & id, vector<string> componentNames)
{
	map<string, shared_ptr<Component::Component>> components;
	for (string & componentName : componentNames) {
		components.insert(std::pair<string, shared_ptr<Component::Component>>(componentName, GetComponent(id, componentName)));
	}
	return components;
}

map<unsigned long, shared_ptr<Component::Component>> EntityManager::GetComponents(const unsigned int & id, unsigned long componentMask)
{
	map<unsigned long, shared_ptr<Component::Component>> components;

	std::bitset<m_maskSize> masks(componentMask);
	for (int i = 0; i < m_maskSize; i++) {
		if (masks[i]) {
			unsigned long mask = std::pow(2, i);
			components.insert(std::pair<unsigned long, shared_ptr<Component::Component>>(mask, m_prototypes[mask]->GetComponent(id)));
		}
	}
	return components;
}

void EntityManager::AttachComponent(shared_ptr<Component::Component> component)
{
	m_prototypes[ComponentMask(component->GetName())]->Attach(component);
}

vector<shared_ptr<Entity>>& EntityManager::Entities()
{
	return m_entities;
}

vector<shared_ptr<Entity>> EntityManager::Entities(unsigned long componentMask)
{
	vector<shared_ptr<Entity>> subset;
	for (shared_ptr<Entity> & entity : m_entities) {
		LoadComponents(entity, componentMask);
		if (entity->HasComponents(componentMask)) subset.push_back(entity);
	}
	return subset;
}

shared_ptr<Entity> EntityManager::FindEntity(unsigned long componentMask)
{
	for (shared_ptr<Entity> & entity : m_entities) {
		if (entity->HasComponents(componentMask)) return entity;
	}
}

void EntityManager::LoadComponents(shared_ptr<Entity>& entity, unsigned long componentMask)
{
	if (!entity->HasComponents(componentMask)) {
		// add each missing component to the entity
		for (auto & component : GetComponents(entity->ID(), entity->MissingComponents(componentMask))) {
			entity->AddComponent(component.first, component.second);
		}
	}
}

vector<shared_ptr<Entity>> EntityManager::EntitiesContaining(string componentName, unsigned long componentMask)
{
	vector<shared_ptr<Entity>> entities;
	// get each entity ID that has the specified component on disk
	for (string & file : Filesystem::FilesIn(m_directory)) {
		unsigned int id = std::stoi(file);
		// check to see if this entity is being tracked
		shared_ptr<Entity> entity = FindEntity(id);
		if (!entity) {
			entity = shared_ptr<Entity>(new Entity(id));
			// Track a new entity
			m_entities.push_back(entity);
		}
		// Update the entity
		LoadComponents(entity, componentMask);
		entities.push_back(entity);
	}
}



//void ComponentManager::Import(string directory)
//{
//	/*for (std::pair<const string, vector<Component>> & pair : m_components) {
//		std::ifstream ifs(directory + "/" + pair.first);
//		pair
//	}*/
//	for (int i = 0; i < m_componentTypes.size(); i++) {
//		string name = m_componentTypes[i];
//		std::ifstream ifs(directory + "/" + name + ".dat");
//		if (ifs) {
//
//			if (name == "Position") {
//				AddComponent<Position>(ImportComponent<Position>(ifs));
//			}
//			else if (name == "Movement") {
//				AddComponent<Movement>(ImportComponent<Movement>(ifs));
//			}
//			
//		}
//	}
//}

//void ComponentManager::Export(string directory)
//{
//	for (int i = 0; i < m_componentTypes.size(); i++) {
//		string name = m_componentTypes[i];
//		std::ofstream ofs(directory + "/" + name + ".dat");
//		if (ofs) {
//
//			if (name == "Position") {
//				GetComponent<Position>()
//			}
//			else if (name == "Movement") {
//				AddComponent<Movement>(ImportComponent<Movement>(ifs));
//			}
//
//		}
//	}
//}

vector<string> EntityManager::Components(unsigned long componentMask)
{
	vector<string> componentNames;
	
	std::bitset<m_maskSize> mask(componentMask);
	for (int i = 0; i < m_maskSize; i++) {
		if (mask[i]) {
			componentNames.push_back(m_prototypes[i]->GetName());
		}
	}
	return componentNames;
}

vector<unsigned long> EntityManager::ComponentMasks(unsigned long componentMask)
{
	vector<unsigned long> componentMasks;

	std::bitset<m_maskSize> mask(componentMask);
	for (int i = 0; i < m_maskSize; i++) {
		if (mask[i]) {
			componentMasks.push_back(mask[i]);
		}
	}
	return componentMasks;
}

unsigned long EntityManager::ComponentMask(vector<string> components)
{
	unsigned long mask = 0;
	for (string & componentName : components) {
		mask |= ComponentMask(componentName);
	}
	return mask;
}

unsigned long EntityManager::ComponentMask(string component)
{
	unsigned long mask = 0;
	for (int i = 0; i < m_maskSize; i++) {
		if (m_prototypes[i]->GetName() == component) {
			mask |= (unsigned long)std::pow(i, 2);
		}
	}
	return mask;
}


//Entity & EntityManager::AddEntity(unsigned long componentMask)
//{
//	Entity entity(NextID, componentMask);
//	// add the components to the component lists
//	std::bitset<m_maskSize> mask(componentMask);
//	for (int i = 0; i < m_maskSize; i++) {
//		if (mask[i]) {
//			m_prototypes[i]->A(entity.ID);
//		}
//	}
//	// add this entity to the list of entities
//	m_entities.push_back(entity);
//	return entity;
//}

unsigned int EntityManager::NewEntity()
{
	return m_nextID++;
}

EntityManager::EntityManager(const string & directory) : m_directory(directory)
{
	AddPrototype(new Component::Position());
	AddPrototype(new Component::Terrain());
	//AddPrototype(new Movement());
	//AddPrototype(new Input());
	//AddPrototype(new Flag("Player"));
}


EntityManager::~EntityManager()
{
	Save();
}

//void EntityManager::Load(vector<unsigned int> & entities, unsigned long & componentMask)
//{
//	std::bitset<m_maskSize> mask(componentMask);
//	for (int i = 0; i < m_maskSize; i++) {
//		if (mask[i]) {
//			for (unsigned int & id : entities) {
//				m_prototypes[i]->AddComponent(entity.ID);
//			}
//			
//		}
//	}
//}

void EntityManager::Save()
{
	
}

void EntityManager::AddPrototype(Component::Component* prototype)
{
	unsigned long mask = std::pow(2, m_prototypes.size());
	m_prototypes.insert(std::pair<unsigned long, unique_ptr<Component::Component>>(mask, std::make_unique<Component::Component>(prototype)));
	m_masks.insert(std::pair<string, unsigned long>(prototype->GetName(), mask));
}

shared_ptr<Entity> EntityManager::FindEntity(const unsigned int & id)
{
	for (shared_ptr<Entity> & entity : m_entities) {
		if (entity->ID() == id) return entity;
	}
	return shared_ptr<Entity>(nullptr);
}
