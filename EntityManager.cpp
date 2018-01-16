#include "pch.h"
#include "EntityManager.h"
#include "JsonParser.h"
#include <bitset>
// Concrete Component types
#include "Position.h"
#include "Player.h"
#include "VBO.h"
#include "Movement.h"
#include "Tag.h"
#include "Terrain.h"
#include "Model.h"
#include "Entity.h"

const string EntityManager::m_nextEntityFile = "Next_Entity.txt";

EntityManager::EntityManager() : m_nextID(1), m_player(nullptr)
{
	InitializeComponents();
}

EntityManager::EntityManager(Filesystem::path & directory) : EntityManager::EntityManager()
{
	SetDirectory(directory);
}

shared_ptr<Components::Component> EntityManager::GetComponent(unsigned long & mask, EntityPtr entity)
{
	// search the cache
	map<unsigned long, shared_ptr<Components::Component>>::iterator it = entity->Components().find(mask);
	if (!entity->Components().empty() && it != entity->Components().end()) {
		return it->second;
	}
	// load from file
	shared_ptr<Components::Component> component;
	std::size_t underscorePos = m_names[mask].find('_');
	if (underscorePos == string::npos) {
		// Discrete component types
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
		else if (m_names[mask] == "Model") {
			component = shared_ptr<Components::Component>(new Components::Model(entity->ID()));
		}
	}
	else {
		// Deferred types
		string discreteType = m_names[mask].substr(0, underscorePos);	// before the '_'
		string deferredType = m_names[mask].substr(underscorePos+1);	// after the '_'
		if (discreteType == "Tag") {
			component = shared_ptr<Components::Component>(new Components::Tag(entity->ID(),deferredType));
		}
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

vector<EntityPtr> EntityManager::FindEntities(unsigned long componentMask)
{
	
	// search cache
	unordered_set<unsigned int> entities;
	for (unordered_map<unsigned int, EntityPtr>::iterator it = m_entities.begin(); it != m_entities.end(); it++) {
		if (it->second->HasComponents(componentMask)) entities.insert(it->first);
	}
	// search filesysem
	if (Initialized()) {
		unordered_set<unsigned int> unCached;
		std::bitset<m_maskSize> mask(componentMask);
		bool firstComp = true;
		for (int i = 0; i < m_maskSize; i++) {
			if (mask[i]) {
				unordered_set<unsigned int> nextMatching;
				for (auto & dir : Filesystem::directory_iterator(m_directory / m_names[std::pow(2, i)])) {
					unsigned int id = std::stoi(dir.path().filename());
					if (firstComp || unCached.count(id)) nextMatching.insert(id);
				}
				unCached = nextMatching;
				firstComp = false;
			}
		}
		for (auto i : unCached) {
			entities.insert(i);
		}
	}
	// convert to vector
	vector<EntityPtr> entityVector;
	for (const unsigned int & id : entities) {
		EntityPtr entity;
		if (m_entities.count(id)) {
			// get from cache
			entity = m_entities[id];
		} else {
			// cache a new entity
			entity = EntityPtr(new Entity(id, ComponentMaskOf(id)));
			m_entities.insert(std::pair<unsigned int, EntityPtr>(id, entity));
		}
		entityVector.push_back(entity);
	}
	return entityVector;
}

vector<EntityPtr> EntityManager::FindEntitiesInRange(unsigned long componentMask, Vector3 center, float range)
{
	auto entities = FindEntities(componentMask | ComponentMask("Position"));
	vector<EntityPtr> finalSet;
	for (auto & entity : entities) {
		if (Vector3::Distance(GetComponent<Components::Position>(entity, "Position")->Pos,center) <= range) {
			finalSet.push_back(entity);
		}
	}
	return finalSet;
}

bool EntityManager::Find(const unsigned int & id, EntityPtr & entity)
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

EntityPtr EntityManager::Player()
{
	if (!m_player) { 
		auto results = FindEntities(ComponentMask("Player"));
		m_player = results.size() != 0 ? results[0] : nullptr; }
	return m_player;
}

shared_ptr<Components::Position> EntityManager::PlayerPos()
{
	return GetComponent<Components::Position>(Player(), "Position");
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

EntityPtr EntityManager::NewEntity()
{
	EntityPtr entity = EntityPtr(new Entity(m_nextID++, 0));
	m_entities.insert(std::pair<unsigned int, EntityPtr>(entity->ID(),entity));
	return entity;
}

EntityManager::~EntityManager()
{
	//Save();
}

bool EntityManager::Initialized()
{
	return !m_directory.empty();
}

void EntityManager::UnInitialize()
{
	m_directory = "";
	// clear the cache
	m_entities.clear();
}

void EntityManager::SetDirectory(Filesystem::path & directory)
{
	m_directory = directory;
	//----------------------------------------------------------------
	// Initialize filesystem dependencies

	Filesystem::create_directory(m_directory);
	Components::Component::SetDirectory(m_directory);
	ifstream nextEntityFile(m_directory / m_nextEntityFile);
	if (nextEntityFile) {
		nextEntityFile >> m_nextID;
	}
}

void EntityManager::InitializeComponents()
{
	//----------------------------------------------------------------
	// Initialize the component mappings
	AddComponentVector("Player");

	AddComponentVector("Position");
	AddComponentVector("Movement");

	AddComponentVector("Terrain");

	AddComponentVector("VBO");
	AddComponentVector("Model");

	//----------------------------------------------------------------
	// Tags
	AddComponentVector("Tag_Water");
	AddComponentVector("Tag_Tree");
}

void EntityManager::Save()
{
	//----------------------------------------------------------------
	// Entities
	for (std::unordered_map<unsigned int, EntityPtr>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
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
}

