#include "pch.h"
#include "EntityManager.h"
#include "JsonParser.h"
#include <bitset>
// Concrete Component types
#include "Position.h"
#include "Player.h"
#include "PositionNormalTextureVBO.h"
#include "Movement.h"
#include "Tag.h"
#include "Terrain.h"
#include "Model.h"
#include "Entity.h"

const Filesystem::path EntityManager::m_nextEntityFile("Next_Entity.txt");


EntityManager::EntityManager(Filesystem::path & directory) : BaseEntityManager::BaseEntityManager(), m_player(nullptr), m_directory(directory)
{
	//----------------------------------------------------------------
	// Initialize filesystem dependencies
	ifstream nextEntityFile(m_directory / m_nextEntityFile);
	if (nextEntityFile) {
		unsigned int nextID = 1;
		nextEntityFile >> nextID;
		SetNextID(nextID);
	}
	//----------------------------------------------------------------
	// Register the components
	RegisterComponent([] {return new Components::Player();});

	RegisterComponent([] {return new Components::Position();});
	RegisterComponent([] {return new Components::Movement();});

	RegisterComponent([] {return new Components::Terrain();});

	RegisterComponent([] {return new Components::PositionNormalTextureVBO();});
	RegisterComponent([] {return new Components::Model();});

	//----------------------------------------------------------------
	// Tags
	RegisterDelegate([](string type) {return new Components::Tag(type);}, vector<string>{
		"Water", 
		"Tree"
	});
}

shared_ptr<Components::Component> EntityManager::LoadComponent(unsigned long & mask, Entity * entity)
{
	shared_ptr<Components::Component> component = std::shared_ptr<Components::Component>(GetPrototype(mask));
	//std::size_t underscorePos = m_names[mask].find('_');
	//if (underscorePos == string::npos) {
	//}
	//else {
	//	// Delegate types
	//	string discreteType = m_names[mask].substr(0, underscorePos);	// before the '_'
	//	string delegateType = m_names[mask].substr(underscorePos+1);	// after the '_'
	//	component = std::shared_ptr<Components::Component>(m_delegatePrototypes[mask](delegateType));
	//}

	if (component) {
		component->Load(m_directory, entity->ID());
		return component;
	}
	else {
		// not found
		return nullptr;
	}
}



vector<EntityPtr> EntityManager::LoadEntities(unsigned long & componentMask)
{
	// search filesysem
	unordered_set<unsigned int> unCached;
	std::bitset<m_maskSize> mask(componentMask);
	bool firstComp = true;
	for (int i = 0; i < m_maskSize; i++) {
		if (mask[i]) {
			unordered_set<unsigned int> nextMatching;
			for (auto & dir : Filesystem::directory_iterator(m_directory / NameOf(std::pow(2, i)))) {
				unsigned int id = std::stoi(dir.path().filename());
				if (firstComp || unCached.count(id)) nextMatching.insert(id);
			}
			unCached = nextMatching;
			firstComp = false;
		}
	}
	// convert to Entity pointers
	vector<EntityPtr> entityVector;
	for (const unsigned int & id : unCached) {
		if (!m_entities.count(id)) {
			entityVector.push_back(EntityPtr(new Entity(id, ComponentMaskOf(id), this)));
		}
	}
	return entityVector;
}

//----------------------------------------------------------------
// Very slow performance ~ 53% CPU

//vector<EntityPtr> EntityManager::LoadEntities(unsigned long & componentMask)
//{
//	// search filesysem
//	map<unsigned int, unsigned long> results;
//	std::bitset<m_maskSize> mask(componentMask);
//	// find seed component (the first required component)
//	int seed = 0;
//	for (int i = 0; i < m_maskSize; i++) {
//		if (mask[i]) {
//			seed = i;
//			break;
//		}
//	}
//	// initialize results from seed component
//	unsigned long seedMask = std::pow(2, seed);
//	string seedName = NameOf(seedMask);
//	for (auto & compFile : Filesystem::directory_iterator(m_directory / seedName)) {
//		unsigned int id = std::stoi(compFile.path().filename());
//		results.insert(std::make_pair(id, seedMask));
//	}
//	// filter and reduce reults
//	for (int i = 0; i < m_maskSize; i++) {
//		unsigned long thisMask = std::pow(2, i);
//		string maskName = NameOf(thisMask);
//		// only operate on known intersections
//		for (auto it = results.begin(); it != results.end();) {
//			// Update mask
//			if (Filesystem::exists(m_directory / maskName / (to_string(it->first) + ".dat"))) {
//				it->second |= thisMask;
//			}
//			// 'Yank' from the results
//			else if (mask[i]) {
//				it = results.erase(it);
//			}
//			++it;
//		}
//	}
//	// convert to Entity pointers
//	vector<EntityPtr> entityVector;
//	for (auto & result : results) {
//		if (!m_entities.count(result.first)) {
//			entityVector.push_back(EntityPtr(new Entity(result.first, result.second, this)));
//		}
//	}
//	return entityVector;
//}

//----------------------------------------------------------------
// Slow ~ 9% CPU

//vector<EntityPtr> EntityManager::LoadEntities(unsigned long & componentMask)
//{
//	// search filesysem
//	unordered_set<unsigned int> results;
//	std::bitset<m_maskSize> mask(componentMask);
//
//	bool firstComp = true;
//	// filter and reduce results
//	for (int i = 0; i < m_maskSize; i++) {
//		if (mask[i]) {
//			unsigned long thisMask = std::pow(2, i);
//			string maskName = NameOf(thisMask);
//			if (firstComp) {
//				for (auto & compFile : Filesystem::directory_iterator(m_directory / maskName)) {
//					unsigned int id = std::stoi(compFile.path().filename());
//					results.insert(id);
//				}
//			}
//			else {
//				// only operate on known intersections
//				for (auto it = results.begin(); it != results.end();) {
//					// Update mask
//					if (Filesystem::exists(m_directory / maskName / (to_string(*it) + ".dat"))) {
//						++it;
//					}
//					// 'Yank' from the results
//					else {
//						it = results.erase(it);
//					}
//				}
//			}
//			firstComp = false;
//		}
//	}
//	// convert to Entity pointers
//	vector<EntityPtr> entityVector;
//	for (auto & result : results) {
//		if (!m_entities.count(result)) {
//			entityVector.push_back(EntityPtr(new Entity(result, ComponentMaskOf(result), this)));
//		}
//	}
//	return entityVector;
//}

vector<EntityPtr> EntityManager::FindEntitiesInRange(unsigned long componentMask, Vector3 center, float range)
{
	auto entities = FindEntities(componentMask | ComponentMask("Position"));
	vector<EntityPtr> finalSet;
	for (auto & entity : entities) {
		if (Vector3::Distance(entity->GetComponent<Components::Position>("Position")->Pos,center) <= range) {
			finalSet.push_back(entity);
		}
	}
	return finalSet;
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
	return Player()->GetComponent<Components::Position>("Position");
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

EntityManager::~EntityManager()
{
	//Save();
}

void EntityManager::Save()
{
	//----------------------------------------------------------------
	// Components
	for (std::unordered_map<unsigned int, EntityPtr>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
		for (auto & component : it->second->GetComponents()) {
			component->Save(m_directory);
		}
	}
	//----------------------------------------------------------------
	// Next Entity ID
	ofstream nextEntityFile(m_directory / m_nextEntityFile);
	nextEntityFile << GetNextID();
}

