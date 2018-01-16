#pragma once
#include "Entity.h"
class BaseEntityManager
{
public:
	BaseEntityManager();
	~BaseEntityManager();
	//----------------------------------------------------------------
	// Component retrieval 
	template <typename CompType>
	inline shared_ptr<CompType> GetComponent(EntityPtr entity, string name) {
		return std::dynamic_pointer_cast<CompType>(GetComponent(m_masks[name], entity));
	}
	//----------------------------------------------------------------
	// Mask helpers
	unsigned long ComponentMask(vector<string> components);
	unsigned long ComponentMask(string component);
	virtual unsigned long ComponentMaskOf(const unsigned int & id);
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
	void AddComponentVector(string name);
	virtual shared_ptr<Components::Component> InstantiateComponent(unsigned long & mask, EntityPtr entity) = 0;
	// Name, mask
	map<string, unsigned long> m_masks;
	map<unsigned long, string> m_names;
	//----------------------------------------------------------------
	// Entity caching
	unordered_map<unsigned int, EntityPtr> m_entities;
	// Loads the secified component for the entity
	shared_ptr<Components::Component> GetComponent(unsigned long & mask, EntityPtr entity);
private:
	const Filesystem::path m_directory;
	//----------------------------------------------------------------
	// ID management
	unsigned int m_nextID;
	// Mask, index
	map<unsigned long, unsigned int> m_indices;
	static const int m_maskSize = 64;
	

};

