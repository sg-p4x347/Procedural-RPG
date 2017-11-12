#include "pch.h"
#include "EntityManager.h"
#include "JsonParser.h"
#include <bitset>
// Concrete Component types
#include "Position.h"
#include "Player.h"
#include "VBO.h"
#include "Movement.h"

#include "Terrain.h"

const string EntityManager::m_nextEntityFile = "Next_Entity.txt";
EntityManager::EntityManager(Filesystem::path & directory) : m_directory(directory), m_nextID(1), m_player(nullptr)
{
	//----------------------------------------------------------------
	// Initialize filesystem dependencies

	Filesystem::create_directory(m_directory);
	Components::Component::SetDirectory(m_directory);
	ifstream nextEntityFile(m_directory / m_nextEntityFile);
	if (nextEntityFile) {
		nextEntityFile >> m_nextID;
	}

		
	//----------------------------------------------------------------
	// Initialize the component mappings

	AddComponentVector("Position");
	AddComponentVector("Terrain");
	AddComponentVector("VBO");
	AddComponentVector("Player");
	AddComponentVector("Movement");

}

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

	/*std::bitset<m_maskSize> masks(componentMask);
	for (int i = 0; i < m_maskSize; i++) {
		if (masks[i]) {
			unsigned long mask = std::pow(2, i);
			components.insert(std::pair<unsigned long, shared_ptr<Components::Component>>(mask, m_prototypes[mask]->GetComponent(id)));
		}
	}*/
	return components;
}
shared_ptr<Components::Component> EntityManager::GetComponent(unsigned long & mask, shared_ptr<Entity> entity)
{
	// search the cache
	map<unsigned long, shared_ptr<Components::Component>>::iterator it = entity->Components().find(mask);
	if (!entity->Components().empty() && it != entity->Components().end()) {
		return it->second;
	}
	// load from file
	shared_ptr<Components::Component> component;
	if (m_names[mask] == "VBO") {
		component = shared_ptr<Components::Component>(new Components::VBO(entity->ID()));
	}
	else if (m_names[mask] == "Position") {
		component = shared_ptr<Components::Component>(new Components::Position(entity->ID()));
	}
	else if (m_names[mask] == "Player") {
		component = shared_ptr<Components::Component>(new Components::Player(entity->ID()));
	}
	else if (m_names[mask] == "Terrain") {
		component = shared_ptr<Components::Component>(new Components::Terrain(entity->ID()));
	}
	else if (m_names[mask] == "Movement") {
		component = shared_ptr<Components::Component>(new Components::Movement(entity->ID()));
	}
	if (component) {
		component->Load(m_directory, entity->ID());

		entity->AddComponent(mask, component);
		return component;
	}
	else {
		// not found
		return nullptr;
	}
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

//vector<unsigned int>& EntityManager::Entities()
//{
//	return m_entities;
//}

//vector<unsigned int> EntityManager::Entities(unsigned long componentMask)
//{
//	vector<unsigned int> subset;
//	for (unsigned int & entity : m_entities) {
//		if (HasComponents(entity,componentMask)) subset.push_back(entity);
//	}
//	return subset;
//}

vector<shared_ptr<Entity>> EntityManager::FindEntities(unsigned long componentMask)
{
	unordered_set<unsigned int> entities;
	// search cache
	for (unordered_map<unsigned int, shared_ptr<Entity>>::iterator it = m_entities.begin(); it != m_entities.end(); it++) {
		if (it->second->HasComponents(componentMask)) entities.insert(it->first);
	}
	// search filesysem
	std::bitset<m_maskSize> mask(componentMask);
	bool firstComp = true;
	for (int i = 0; i < m_maskSize; i++) {
		if (mask[i]) {
			unordered_set<unsigned int> nextMatching;
			for (auto & dir : Filesystem::directory_iterator(m_directory / m_names[std::pow(2,i)])) {
				unsigned int id = std::stoi(dir.path().filename());
				if (firstComp || entities.count(id)) nextMatching.insert(id);
			}
			entities = nextMatching;
			firstComp = false;
		}
	}
	// convert to vector
	vector<shared_ptr<Entity>> entityVector;
	for (const unsigned int & id : entities) {
		shared_ptr<Entity> entity;
		if (m_entities.count(id)) {
			// get from cache
			entity = m_entities[id];
		} else {
			// cache a new entity
			entity = shared_ptr<Entity>(new Entity(id, ComponentMaskOf(id)));
			m_entities.insert(std::pair<unsigned int, shared_ptr<Entity>>(id, entity));
		}
		entityVector.push_back(entity);
	}
	return entityVector;
}

//void EntityManager::LoadComponents(unsigned long componentMask)
//{
//	std::bitset<m_maskSize> mask(componentMask);
//	for (int i = 0; i < m_maskSize; i++) {
//		if (mask[i]) {
//			componentNames.push_back(m_prototypes[i]->GetName());
//		}
//	}
//}

shared_ptr<Entity> EntityManager::Player()
{
	if (!m_player) { m_player = FindEntities(ComponentMask("Player"))[0]; }
	return m_player;
}

//vector<unsigned int> EntityManager::EntitiesContaining(string componentName)
//{
//	vector<unsigned int> entities;
//	vector<shared_ptr<Components::Component>> & components = m_components[m_indices[m_masks[componentName]]];
//	for (auto & component : components) {
//		entities.push_back(component->ID);
//	}
//	// get each entity ID that has the specified component on disk
//	/*for (auto & dir : Filesystem::directory_iterator(m_directory / componentName)) {
//		entities.push_back(std::stoi(dir.path().filename()));
//	}*/
//	return entities;
//}



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

//vector<string> EntityManager::Components(unsigned long componentMask)
//{
//	/*vector<string> componentNames;
//	
//	std::bitset<m_maskSize> mask(componentMask);
//	for (int i = 0; i < m_maskSize; i++) {
//		if (mask[i]) {
//			componentNames.push_back(m_prototypes[i]->GetName());
//		}
//	}
//	return componentNames;*/
//}
//
//vector<unsigned long> EntityManager::ComponentMasks(unsigned long componentMask)
//{
//	vector<unsigned long> componentMasks;
//
//	std::bitset<m_maskSize> mask(componentMask);
//	for (int i = 0; i < m_prototypes.size(); i++) {
//		if (mask[i]) {
//			componentMasks.push_back(mask[i]);
//		}
//	}
//	return componentMasks;
//}
//
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
	return m_masks[component];
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

shared_ptr<Entity> EntityManager::NewEntity()
{
	shared_ptr<Entity> entity = shared_ptr<Entity>(new Entity(m_nextID++, 0));
	m_entities.insert(std::pair<unsigned int, shared_ptr<Entity>>(entity->ID(),entity));
	return entity;
}




EntityManager::~EntityManager()
{
	//Save();
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
	//----------------------------------------------------------------
	// Components
	/*for (vector<shared_ptr<Components::Component>> compVector : m_components) {
		for (auto & component : compVector) {
			component->Save(m_directory);
		}
	}*/
	//----------------------------------------------------------------
	// Entities
	for (std::unordered_map<unsigned int, shared_ptr<Entity>>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
		it->second->Save(m_directory);
	}
	//----------------------------------------------------------------
	// Next Entity ID
	ofstream nextEntityFile(m_directory / m_nextEntityFile);
	nextEntityFile << m_nextID;
}

void EntityManager::AddComponentVector(string name)
{
	static unsigned int index = 0;

	unsigned long mask = std::pow(2, index++);
	m_masks.insert(std::pair<string, unsigned long>(name, mask));
	m_names.insert(std::pair<unsigned long, string>(mask, name));
	//m_indices.insert(std::pair<unsigned long, unsigned int>(mask, index));
	//m_components.push_back(vector<shared_ptr<Components::Component>>());
}

