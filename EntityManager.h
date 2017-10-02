#pragma once

#include "Entity.h"
#include "Region.h"

#include "Filesystem.h"

using std::vector;

class EntityManager
{
public:
	EntityManager(const string & directory);
	~EntityManager();
	// Load and Save components of entities that satisfy the mask
	void Load(vector<unsigned int> & entities, unsigned long & componentMask);
	void Save();
	// Get entity components
	shared_ptr<Component::Component> GetComponent(const unsigned int & id, string componentName);
	shared_ptr<Component::Component> GetComponent(const unsigned int & id, unsigned long componentMask);
	map<string,shared_ptr<Component::Component>> GetComponents(const unsigned int & id, vector<string> componentNames);
	map<unsigned long, shared_ptr<Component::Component>> GetComponents(const unsigned int & id, unsigned long componentMask);
	// Add entity components
	void AttachComponent(shared_ptr<Component::Component> component);
	
	// Get entities
	vector<shared_ptr<Entity>> & Entities();
	vector<shared_ptr<Entity>> Entities(unsigned long componentMask);
	shared_ptr<Entity> FindEntity(unsigned long componentMask);
	void LoadComponents(shared_ptr<Entity> & entity, unsigned long componentMask);
	// loads
	vector<shared_ptr<Entity>> EntitiesContaining(string componentName, unsigned long componentMask);
	//template <typename CompType>
	//inline unique_ptr<CompType> GetComponent(const unsigned int & id) {
	//	return std::make_unique<CompType>(static_cast<CompType*>(CompType::GetComponent(id)));
	//}
	
	// Component mapping
	vector<string> Components(unsigned long componentMask);
	vector<unsigned long> ComponentMasks(unsigned long componentMask);
	unsigned long ComponentMask(vector<string> components);
	unsigned long ComponentMask(string component);
	// Entity factories
	unsigned int NewEntity();
	void AddPlayer(string name);
private:
	const string m_directory;
	// Manage Entity IDs
	unsigned int m_ID;
	unsigned int m_nextID;

	void AddPrototype(Component::Component* prototype);
	map<unsigned long, unique_ptr<Component::Component>> m_prototypes;
	map<string, unsigned long> m_masks;
	static const int m_maskSize = 64;
	// Entity list
	shared_ptr<Entity> FindEntity(const unsigned int & id);
	vector<shared_ptr<Entity>> m_entities;
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

	
};

