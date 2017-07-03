#pragma once
#include "Region.h"
#include "Entity.h"
#include "Position.h"
#include "Movement.h"

using std::vector;

class EntityManager
{
public:
	EntityManager(const string & directory);
	~EntityManager();
	static EntityManager & Instance(string directory = "");
	// Load and Save regions of entities
	void Load(Region & region);
	void Save(Region & region);
	// Get entity components
	vector<unique_ptr<Component>> GetComponents(const unsigned int & id, vector<string> & compNames);
	template <typename CompType>
	inline unique_ptr<CompType> GetComponent(const unsigned int & id) {
		return std::make_unique<CompType>(static_cast<CompType*>(CompType::GetComponent(id)));
	}
	// Entity factories
	Entity & AddEntity(vector<string> & components);
	void AddPlayer(string name);
private:
	
	
	const string m_directory;
	// Manage Entity IDs
	unsigned int m_ID;
	// Load and Save manager data
	void Load();
	void Save();
	unsigned int NextID;

	//// Importing components
	//template <typename CompType>
	//inline CompType ImportComponent(std::ifstream & ifs) {
	//	return CompType(ifs);
	//}
	//// Adding components
	//template <typename CompType>
	//inline void AddComponent(CompType & component) {
	//	GetComponents<CompType>().push_back(component);
	//}
	//// Get specific component vector
	//template <typename CompType>
	//inline vector<CompType> & GetComponents() {
	//	return std::get<vector<CompType>>(m_components);
	//}
	//// Get component
	//template <typename CompType>
	//inline bool GetComponent(unsigned int & id, CompType & comp) {
	//	for (CompType & component : GetComponents<CompType>()) {
	//		if (component.ID == id) {
	//			comp = component;
	//			return true;
	//		}
	//		else if (component.ID > id) {
	//			return false;
	//		}
	//	}
	//	return false;
	//}
	// Component lists
	//std::map<string,vector<Component>*> m_components;
	//std::tuple<
	//	vector<Position>,
	//	vector<Movement>
	//> m_components;
	//// Component vector positions
	//vector<string> m_componentTypes = vector<string>({
	//	"Position",
	//	"Movement"
	//});

	map<string, Component *> m_prototypes;
	// Entity list
	vector<Entity> m_entities;
};

