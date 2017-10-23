#include "pch.h"
#include "EntityManager.h"
#include "JsonParser.h"
#include <bitset>
// Concrete Component types
#include "Position.h"
//#include "Movement.h"
//#include "Input.h"
//#include "Flag.h"
#include "Terrain.h"

shared_ptr<Components::Component> EntityManager::GetComponent(const unsigned int & id, string componentName)
{
	return GetComponent(id, m_masks[componentName]);
}

shared_ptr<Components::Component> EntityManager::GetComponent(const unsigned int & id, unsigned long componentMask)
{
	/*for (auto & component : m_components[m_masks[componentMask]]) {
		if (component->ID == id && ) {
			return component;
		}
	}*/
	return nullptr;
	/*shared_ptr<Components::Component> component = m_prototypes[componentMask]->GetComponent(id);
	if (!component) component = m_prototypes[componentMask]->Load(m_directory.string(), id);
	return component;*/
}

map<string, shared_ptr<Components::Component>> EntityManager::GetComponents(const unsigned int & id, vector<string> componentNames)
{
	map<string, shared_ptr<Components::Component>> components;
	for (string & componentName : componentNames) {
		components.insert(std::pair<string, shared_ptr<Components::Component>>(componentName, GetComponent(id, componentName)));
	}
	return components;
}

map<unsigned long, shared_ptr<Components::Component>> EntityManager::GetComponents(const unsigned int & id, unsigned long componentMask)
{
	map<unsigned long, shared_ptr<Components::Component>> components;

	std::bitset<m_maskSize> masks(componentMask);
	for (int i = 0; i < m_maskSize; i++) {
		if (masks[i]) {
			unsigned long mask = std::pow(2, i);
			components.insert(std::pair<unsigned long, shared_ptr<Components::Component>>(mask, m_prototypes[mask]->GetComponent(id)));
		}
	}
	return components;
}
shared_ptr<Components::Component> EntityManager::AddComponent(const unsigned int & id, unsigned long componentMask)
{
	return m_prototypes[(int)std::log2(componentMask)]->Add(id);
}
bool EntityManager::HasComponents(const unsigned int & id, unsigned long & componentMask)
{
	return componentMask == (componentMask & ComponentMaskOf(id));
}

unsigned long EntityManager::MissingComponents(const unsigned int & id, unsigned long & componentMask)
{
	// first xor yields difference, second removes extra components
	return (componentMask ^ ComponentMaskOf(id)) ^ ComponentMaskOf(id);
}

vector<unsigned int>& EntityManager::Entities()
{
	return m_entities;
}

vector<unsigned int> EntityManager::Entities(unsigned long componentMask)
{
	vector<unsigned int> subset;
	for (unsigned int & entity : m_entities) {
		if (HasComponents(entity,componentMask)) subset.push_back(entity);
	}
	return subset;
}

unsigned int EntityManager::FindEntity(unsigned long componentMask)
{
	for (unsigned int & entity : m_entities) {
		if (HasComponents(entity,componentMask)) return entity;
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

unsigned int EntityManager::Player()
{
	if (!m_player) {
		m_player = FindEntity(ComponentMask("Player"));
	}
	return m_player;
}

vector<unsigned int> EntityManager::EntitiesContaining(string componentName, unsigned long componentMask)
{
	vector<unsigned int> entities;
	// get each entity ID that has the specified component on disk
	for (string & file : FileSystemHelpers::FilesIn(m_directory.string())) {
		entities.push_back(std::stoi(file));
	}
	return entities;
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
	for (int i = 0; i < m_prototypes.size(); i++) {
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
	for (int i = 0; i < m_prototypes.size(); i++) {
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
	for (int i = 0; i < m_prototypes.size(); i++) {
		unsigned long mask = (unsigned long)std::pow(2, i);
		if (m_prototypes[i]->GetName() == component) {
			return mask;
		}
	}
	return 0;
}

unsigned long EntityManager::ComponentMaskOf(const unsigned int & id)
{
	unsigned long mask = 0;
	for (auto& component : Filesystem::directory_iterator(m_directory)) {
		if (Filesystem::exists(component.path() / (std::to_string(id) + ".dat"))) {
			mask |= ComponentMask(component.path().filename().string());
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

EntityManager::EntityManager(Filesystem::path & directory) : m_directory(directory / "Component"), m_nextID(0)
{
	Filesystem::create_directory(m_directory);

	AddComponentVector("Position");
	AddComponentVector("Terrain");
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

void EntityManager::AddComponentVector(string name)
{
	unsigned int index = m_components.size();
	unsigned long mask = std::pow(2, index);
	m_masks.insert(std::pair<string, unsigned long>(name, mask));
	m_indices.insert(std::pair<unsigned long, unsigned int>(mask, index));
	m_components.push_back(vector<shared_ptr<Components::Component>>());
}

