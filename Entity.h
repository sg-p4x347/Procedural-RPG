#pragma once
#include "Component.h"
#include "BaseEntityManager.h"

class Entity {
public:
	Entity(const unsigned int & id, const unsigned long & mask,BaseEntityManager * entityManager);
	//----------------------------------------------------------------
	// Getters
	unsigned int ID();
	unsigned long GetMask();
	bool IsRemoved();
	//----------------------------------------------------------------
	// Mask Queries
	bool HasComponents(const unsigned long & mask);
	unsigned long MissingComponents(const unsigned long & mask);
	//----------------------------------------------------------------
	// Component retrieval
	template <typename CompType>
	inline bool TryGetComponent(string name, shared_ptr<CompType> & component) {
		component = GetComponent<CompType>(name);
		return component != nullptr;
	}
	template <typename CompType>
	inline shared_ptr<CompType> GetComponent(string name) {
		if (m_removed) {
			auto test = 7;
		}
		unsigned long componentMask = m_entityManager->ComponentMask(name);
		if (HasComponents(componentMask)) {
			if (m_components.find(componentMask) != m_components.end()) {
				return dynamic_pointer_cast<CompType>(m_components[componentMask]);
			}
			else {
				return dynamic_pointer_cast<CompType>(m_entityManager->GetComponent(componentMask, this));
			}
		}
		else {
			return nullptr;
		}
	}
	//template <typename CompType>
	//inline shared_ptr<CompType> GetComponent() {
	//	// get the mask
	//	
	//}
	vector<shared_ptr<Components::Component>> GetComponents();
	//----------------------------------------------------------------
	// Component addition
	void AddComponent(Components::Component * component);
	void AddComponent(shared_ptr<Components::Component> & component);
	//----------------------------------------------------------------
	// Component removal
	void RemoveComponents(unsigned long mask);
	void RemoveComponents();
	void RemoveEntity();
private:
	const unsigned int m_id;
	unsigned long m_mask;
	bool m_removed;
	map<unsigned long, shared_ptr<Components::Component>> m_components;
	BaseEntityManager * m_entityManager;
};