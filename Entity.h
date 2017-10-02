#pragma once
#include "JSON.h"
#include "Component.h"
#include "JSON.h"



class Entity : JSON {
public:
	Entity(unsigned int id);
	unsigned int ID();
	unsigned long ComponentMask();
	bool HasComponents(unsigned long & componentMask);
	unsigned long MissingComponents(unsigned long & componentMask);

	void AddComponent(unsigned long componentMask, shared_ptr<Component::Component> component);
	map<string, shared_ptr<Component::Component>>& GetComponents();
	template <typename T>
	inline shared_ptr<T>  GetComponent(string componentName) {
		shared_ptr<Component::Component> component = m_components[componentName];
		return shared_ptr<T>(dynamic_cast<T>(component));
	}
	// Inherited via JSON
	virtual void Import(JsonParser & jp) override;
	virtual JsonParser Export() override;
private:
	unsigned int m_id;
	unsigned long m_componentMask;
	map<string, shared_ptr<Component::Component>> m_components;

	
};

