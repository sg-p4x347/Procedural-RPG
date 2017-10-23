#pragma once
#include "JSON.h"
#include "Component.h"
#include "JSON.h"



class Entity {
public:
	Entity(unsigned int id);
	unsigned int ID();
	unsigned long ComponentMask();
	bool HasComponents(unsigned long & componentMask);
	unsigned long MissingComponents(unsigned long & componentMask);

	void AddComponent(unsigned long componentMask, shared_ptr<Components::Component> component);
	map<string, shared_ptr<Components::Component>>& GetComponents();
	/*template <typename T>
	inline shared_ptr<T>  GetComponent(string componentName) {
		shared_ptr<Components::Component> component = m_components[componentName];
		return shared_ptr<T>(dynamic_cast<T>(component));
	}*/
private:
	unsigned int m_id;
	unsigned long m_componentMask;
	map<string, shared_ptr<Components::Component>> m_components;

	
};

