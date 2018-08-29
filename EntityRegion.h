#pragma once
#include "Component.h"
#include "MappedFile.h"
#include <streambuf>

typedef shared_ptr<vector<shared_ptr<Components::Component>>> CompCache;
class BaseEntityManager;
class EntityRegion:
{
public:
	EntityRegion(Filesystem::path path, unsigned int x, unsigned int z,BaseEntityManager * entityManager);
	// Loads the component mask for all entities that contain the component mask
	vector<CompCache> LoadCaches(unsigned long componentMask);
	// Loads the specified signature for all entities that contain the component mask
	vector<CompCache> LoadCaches(unsigned long componentMask, unsigned long entitySignature);
	// Save
	void SaveCaches();
private:
	Filesystem::path m_path;
	unsigned int m_x;
	unsigned int m_z;
	//----------------------------------------------------------------
	// Component Cache
	std::map<unsigned long, // Map by component
		std::map<unsigned long, // Map by entity signature
			CompCache
		>
	> m_componentCache;
	std::map<unsigned int, EntityPtr> m_entities;
	std::map<unsigned long, MappedFile<unsigned long>> m_fileIndexes;
	BaseEntityManager * m_em;
private:
	// Streams the string data into a cache of components
	CompCache LoadCache(string && data, unsigned long entitySignature, unsigned long componentMask);
	// Streams the cache of components into a string data
	string SaveCache(CompCache & cache, unsigned long entitySignature, unsigned long componentMask);

};

