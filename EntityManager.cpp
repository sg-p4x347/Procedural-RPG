#include "pch.h"
#include "EntityManager.h"


EntityManager & EntityManager::Instance(string directory)
{
	static EntityManager manager(directory);
	return manager;
}

void EntityManager::Load(Region & region)
{
	for (auto & pair : m_prototypes) {
		pair.second->Load(region.GetDirectory());
	}
}

void EntityManager::Save(Region & region)
{
	// save only components belonging to entities physically located
	// within the specified region
	for (Entity & entity : m_entities) {
		unique_ptr<Position> position = GetComponent<Position>(entity.ID);
		if (region.GetArea().Contains(Vector2(position->Pos.x, position->Pos.z))) {
			// this entity is in this region
			// save all of its components
			for (unique_ptr<Component> & component : GetComponents(entity.ID,entity.Components)) {
				component->Save(region.GetDirectory());
			}
		}
	}
}

vector<unique_ptr<Component>> EntityManager::GetComponents(const unsigned int & id, vector<string>& compNames)
{
	vector<unique_ptr<Component>> components;
	for (string & compName : compNames) {
		components.push_back(std::make_unique<Component>(m_prototypes[compName]->GetComponent(id)));
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

Entity & EntityManager::AddEntity(vector<string> & components)
{
	Entity entity(components);
	// add the components to the component lists
	for (string & component : components) {
		m_prototypes[component]->AddComponent(entity.ID);
	}
	// add this entity to the list of entities
	m_entities.push_back(entity);
	return entity;
}

void EntityManager::AddPlayer(string name)
{
	Entity & player = AddEntity(vector<string>({
		"Position",
		"Movement"
	}));
}

EntityManager::EntityManager(const string & directory) : m_directory(directory)
{
	Load();
	m_prototypes["Position"] = new Position(0,XMFLOAT3(0,0,0),XMFLOAT3(0,0,0));
	//m_prototypes["Movement"] = new Movement();
}


EntityManager::~EntityManager()
{
	Save();
}

void EntityManager::Load()
{
	std::ifstream ifs(m_directory + "/EntityManager.json");
	if (ifs) {
		JsonParser obj(ifs);
		m_ID = (unsigned int)obj["id"];
	}
}

void EntityManager::Save()
{
	std::ofstream ofs(m_directory + "/EntityManager.json");
	if (ofs) {
		JsonParser obj;
		obj.Set("id", m_ID);
		// save the region co-ordinates in which the player resides
		unique_ptr<Position> position = GetComponent<Position>
		obj.Set("regionX",
		obj.Export(ofs);
	}
}
