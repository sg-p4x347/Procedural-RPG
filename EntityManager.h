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
	//----------------------------------------------------------------
	// Save all cached components
	void Save();
	//----------------------------------------------------------------
	// Component retrieval
	template <typename CompType>
	inline shared_ptr<CompType> GetComponent(shared_ptr<Entity> entity, string name) {
		return std::dynamic_pointer_cast<CompType>(GetComponent(m_masks[name],entity));
	}
	//----------------------------------------------------------------
	// Mask helpers
	unsigned long ComponentMask(vector<string> components);
	unsigned long ComponentMask(string component);
	unsigned long ComponentMaskOf(const unsigned int & id);
	//----------------------------------------------------------------
	// Entity creation
	shared_ptr<Entity> NewEntity();
	//----------------------------------------------------------------
	// Entity retrieval
	shared_ptr<Entity> Player();
	shared_ptr<Components::Position> PlayerPos();
	vector<shared_ptr<Entity>> FindEntities(unsigned long componentMask);
	vector<shared_ptr<Entity>> FindEntitiesInRange(unsigned long componentMask, Vector3 center, float range);
	bool Find(const unsigned int & id, shared_ptr<Entity> & entity);
private:
	const Filesystem::path m_directory;
	//----------------------------------------------------------------
	// ID management
	unsigned int m_ID;
	unsigned int m_nextID;
	const static string m_nextEntityFile; 
	//----------------------------------------------------------------
	// Component management
	void AddComponentVector(string name);
	// Name, mask
	map<string, unsigned long> m_masks;
	map<unsigned long, string> m_names;
	// Mask, index
	map<unsigned long, unsigned int> m_indices;
	static const int m_maskSize = 64;
	//----------------------------------------------------------------
	// Entity caching
	shared_ptr<Entity> m_player;
	unordered_map<unsigned int, shared_ptr<Entity>> m_entities;
	// Loads the secified component for the entity
	shared_ptr<Components::Component> GetComponent(unsigned long & mask, shared_ptr<Entity> entity);

	
};

