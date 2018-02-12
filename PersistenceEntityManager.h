#pragma once

#include "Entity.h"
#include "Component.h"
#include "Delegate.h"
#include "BaseEntityManager.h"

using std::vector;

class PersistenceEntityManager :
	public BaseEntityManager
{
public:
	PersistenceEntityManager(Filesystem::path & directory);
	//~PersistenceEntityManager();
	
	//----------------------------------------------------------------
	// Save all cached components
	void Save();
	//----------------------------------------------------------------
	// Component retrieval
	//----------------------------------------------------------------
	// Mask helpers
	unsigned long ComponentMaskOf(const unsigned int & id) override;

	
protected:
	//----------------------------------------------------------------
	// Component management
	shared_ptr<Components::Component> LoadComponent(unsigned long & mask, Entity * entity) override;
	//----------------------------------------------------------------
	// Entity management
	vector<EntityPtr> LoadEntities(unsigned long & mask) override;
private:
	//----------------------------------------------------------------
	// Caching

	// Working directory
	Filesystem::path m_directory;
	// Next entity ID file relative to working directory
	const static Filesystem::path m_nextEntityFile; 
	
	// Mask, index
	map<unsigned long, unsigned int> m_indices;
	static const int m_maskSize = 64;
	

};

