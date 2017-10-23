#pragma once

#include "Entity.h"
#include "Position.h"

#include "Filesystem.h"

using std::vector;

class EntityManager
{
public:
	EntityManager(Filesystem::path & directory);
	~EntityManager();
	// Load and Save components of entities that satisfy the mask
	void Load(vector<unsigned int> & entities, unsigned long & componentMask);
	void Save();
	// Get entity components
	shared_ptr<Components::Component> GetComponent(const unsigned int & id, string componentName);
	shared_ptr<Components::Component> GetComponent(const unsigned int & id, unsigned long componentMask);
	map<string,shared_ptr<Components::Component>> GetComponents(const unsigned int & id, vector<string> componentNames);
	map<unsigned long, shared_ptr<Components::Component>> GetComponents(const unsigned int & id, unsigned long componentMask);

	template <typename CompType>
	inline shared_ptr<CompType> GetComponent(string name, const unsigned int & id) {
		return dynamic_pointer_cast<CompType>(m_components[m_indices[m_masks[name]]]);
	}
	// Adds a component to the specified entity
	shared_ptr<Components::Component> AddComponent(const unsigned int & id, unsigned long componentMask);
	// Get entities
	vector<unsigned int> & Entities();
	vector<unsigned int> Entities(unsigned long componentMask);
	void LoadComponents(shared_ptr<Entity> & entity, unsigned long componentMask);
	// specific helpers
	unsigned int Player();
	// loads
	vector<unsigned int> EntitiesContaining(string componentName, unsigned long componentMask);
	
	
	
	
	// Component mapping
	vector<string> Components(unsigned long componentMask);
	vector<unsigned long> ComponentMasks(unsigned long componentMask);
	unsigned long ComponentMask(vector<string> components);
	unsigned long ComponentMask(string component);

	unsigned long ComponentMaskOf(const unsigned int & id);
	bool HasComponents(const unsigned int & id, unsigned long & componentMask);
	unsigned long MissingComponents(const unsigned int & id, unsigned long & componentMask);
	// Entity factories
	unsigned int NewEntity();
private:
	const Filesystem::path m_directory;
	// Manage Entity IDs
	unsigned int m_ID;
	unsigned int m_nextID;

	void AddComponentVector(string name);
	vector<shared_ptr<Components::Component>> m_prototypes;
	// Name, mask
	map<string, unsigned long> m_masks;
	// Mask, index
	map<unsigned long, unsigned int> m_indices;
	static const int m_maskSize = 64;
	// Entity list
	unsigned int FindEntity(unsigned long componentMask);
	vector<unsigned int> m_entities;

	unsigned int m_player;
	// Component type is indexed by the first vector
	vector<vector<shared_ptr<Components::Component>>> m_components;
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

