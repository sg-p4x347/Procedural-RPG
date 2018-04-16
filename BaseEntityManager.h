#pragma once
#include "Component.h"
#include <queue>
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
	vector<unsigned long> ExtractMasks(unsigned long mask);
	virtual unsigned long ComponentMaskOf(const unsigned int & id);
	string NameOf(const unsigned long & mask);
	//----------------------------------------------------------------
	// Entity creation
	EntityPtr NewEntity();
	EntityPtr Copy(Entity * source);
	//----------------------------------------------------------------
	// Entity removal
	virtual void DeleteEntity(EntityPtr & entity);
	//----------------------------------------------------------------
	// Entity retrieval
	vector<EntityPtr> FindEntities(string compName);
	vector<EntityPtr> FindEntities(unsigned long componentMask);
	vector<EntityPtr> FindEntities(vector<unsigned int> ids);
	virtual bool Find(const unsigned int & id, EntityPtr & entity);
	//----------------------------------------------------------------
	// Cache management

	// removes any entities that are stored in the cache but have no other references in
	// the rest of the application
	virtual void CollectGarbage();
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
	// Find all entities that have no outside references
	vector<EntityPtr> UnreferencedEntities();

	//----------------------------------------------------------------
	// ID management
	void SetNextID(unsigned int & id);
	unsigned int GetNextID();
	queue<unsigned int> m_removedIDs;

	// Mask, index
	map<unsigned long, unsigned int> m_indices;
	static const int m_maskSize = 64;
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

