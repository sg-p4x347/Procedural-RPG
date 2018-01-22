#pragma once
#include "Component.h"
class BaseEntityManager
{
public:
	BaseEntityManager();
	~BaseEntityManager();
	//----------------------------------------------------------------
	// Component retrieval 
	// Gets the secified component for the entity
	shared_ptr<Components::Component> GetComponent(unsigned long & mask, Entity * entity);
	//----------------------------------------------------------------
	// Mask helpers
	unsigned long ComponentMask(vector<string> components);
	unsigned long ComponentMask(string component);
	virtual unsigned long ComponentMaskOf(const unsigned int & id);
	string NameOf(const unsigned long & mask);
	//----------------------------------------------------------------
	// Entity creation
	EntityPtr NewEntity();
	//----------------------------------------------------------------
	// Entity retrieval
	vector<EntityPtr> FindEntities(unsigned long componentMask);
	vector<EntityPtr> FindEntities(vector<unsigned int> ids);
	bool Find(const unsigned int & id, EntityPtr & entity);

protected:
	//----------------------------------------------------------------
	// Component management

	// Creates a mask for the component
	void RegisterComponent(std::function<Components::Component*()>&& instantiate);
	void RegisterDelegate(std::function<Components::Component*(string delegateName)>&& instantiate, vector<string> delegateNames);
	
	// If not retrieved from the cache, this is called
	virtual shared_ptr<Components::Component> LoadComponent(unsigned long & mask, Entity * entity);
	
	// returns a new component
	shared_ptr<Components::Component> GetPrototype(unsigned long & mask);

	//----------------------------------------------------------------
	// Entity management
	
	// If not retrieved from the cache, this is called
	virtual vector<EntityPtr> LoadEntities(unsigned long & mask);
	// Entity ID, Entity pointer
	unordered_map<unsigned int, EntityPtr> m_entities;

	//----------------------------------------------------------------
	// ID management
	void SetNextID(unsigned int & id);
	unsigned int GetNextID();
private:
	//----------------------------------------------------------------
	// Component management
	void RegisterComponent(string name);
	//----------------------------------------------------------------
	// ID management
	unsigned int m_nextID;

	//----------------------------------------------------------------
	// Caching

	// Mask, Prototype instantiation lambda
	map<unsigned long, std::function<Components::Component*()>> m_prototypes;
	// Name, Mask
	map<string, unsigned long> m_masks;
	// Mask, Name
	map<unsigned long, string> m_names;
	
	// map locking
	std::shared_mutex m_mutex;
	

};

