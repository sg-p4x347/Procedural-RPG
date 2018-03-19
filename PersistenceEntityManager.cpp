#include "pch.h"
#include "PersistenceEntityManager.h"
#include "JsonParser.h"
#include <bitset>
#include "Entity.h"
#include "Filesystem.h"
const Filesystem::path PersistenceEntityManager::m_nextEntityFile("Next_Entity.txt");


PersistenceEntityManager::PersistenceEntityManager(Filesystem::path & directory) : BaseEntityManager::BaseEntityManager(), m_directory(directory)
{
	//----------------------------------------------------------------
	// Initialize filesystem dependencies
	ifstream nextEntityFile(m_directory / m_nextEntityFile);
	if (nextEntityFile) {
		unsigned int nextID = 1;
		nextEntityFile >> nextID;
		SetNextID(nextID);
	}
}

shared_ptr<Components::Component> PersistenceEntityManager::LoadComponent(unsigned long & mask, Entity * entity)
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



vector<EntityPtr> PersistenceEntityManager::LoadEntities(unsigned long & componentMask)
{
	// search filesysem
	unordered_set<unsigned int> unCached;
	std::bitset<m_maskSize> mask(componentMask);
	bool firstComp = true;
	for (int i = 0; i < m_maskSize; i++) {
		if (mask[i]) {
			unordered_set<unsigned int> nextMatching;
			for (auto & dir : Filesystem::directory_iterator(m_directory / NameOf(std::pow(2, i)))) {
				string fileName = FileSystemHelpers::StripExtension(dir.path().filename().string());
				if (Utility::IsNumeric(fileName)) {
					unsigned int id = std::stoi(fileName);
					if (firstComp || unCached.count(id)) nextMatching.insert(id);
				}
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

//vector<EntityPtr> PersistenceEntityManager::LoadEntities(unsigned long & componentMask)
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

//vector<EntityPtr> PersistenceEntityManager::LoadEntities(unsigned long & componentMask)
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



unsigned long PersistenceEntityManager::ComponentMaskOf(const unsigned int & id)
{
	unsigned long mask = 0;
	 for (auto& component : Filesystem::directory_iterator(m_directory)) {
		if (Filesystem::exists(component.path() / (std::to_string(id) + ".dat"))) {
			mask |= ComponentMask(component.path().filename().string());
		}
	}
	return mask;
}

void PersistenceEntityManager::DeleteEntity(EntityPtr & entity)
{
	string filename = (std::to_string(entity->ID()) + ".dat");
	// Remove these components from the file system
	for (auto& component : Filesystem::directory_iterator(m_directory)) {
		Filesystem::remove_all(component.path() / filename);
	}
	BaseEntityManager::DeleteEntity(entity);
}

//PersistenceEntityManager::~PersistenceEntityManager()
//{
//	//Save();
//}

void PersistenceEntityManager::Save()
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

