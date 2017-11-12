#pragma once
#include "Component.h"


class Entity {
public:
	Entity(unsigned int id, unsigned long componentMask);
	unsigned int ID();
	unsigned long ComponentMask();
	unsigned long CachedMask();

	bool HasComponents(unsigned long componentMask);
	unsigned long MissingComponents(unsigned long & componentMask);
	/*template <typename CompType>
	inline shared_ptr<CompType> GetComponent(string name) {
		return dynamic_pointer_cast<CompType>(GetComponent(EM->ComponentMask(name)));
	}*/

	//----------------------------------------------------------------
	// Used by EntityManager
	void AddComponent(unsigned long mask, shared_ptr<Components::Component> component);
	void Save(Filesystem::path directory);
	map<unsigned long, shared_ptr<Components::Component>>  & Components();
private:
	unsigned int m_id;
	unsigned long m_componentMask;
	unsigned long m_cachedMask;
	map<unsigned long, shared_ptr<Components::Component>> m_components;
	
	//----------------------------------------------------------------
	// Helper
	//shared_ptr<Components::Component> GetComponent(unsigned long & mask);
};

