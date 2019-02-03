#include "pch.h"
#include "EntityRegion.h"
#include "BaseEntityManager.h"
#include "Entity.h"
EntityRegion::EntityRegion(Filesystem::path path, unsigned int x, unsigned int z, BaseEntityManager * entityManager) : m_path(path), m_x(x), m_z(z), m_em(entityManager)
{
	
	for (auto & mask : m_em->GetMasks()) {
		// Load all component indexes
		Filesystem::path indexPath = path / (mask.first + ".index");
		Filesystem::path dataPath = path / (mask.first + ".index");
		m_fileIndexes.insert(std::make_pair(mask.second, MappedFile<unsigned long>(indexPath.string(), dataPath.string())));
		// Initialize the master component cache
		m_componentCache.insert(std::make_pair(mask.second, std::map<unsigned long, CompCache>()));
	}
}

vector<CompCache> EntityRegion::LoadCaches(unsigned long componentMask)
{
	return LoadCaches(componentMask, componentMask);
}


vector<CompCache> EntityRegion::LoadCaches( unsigned long componentMask, unsigned long entitySignature)
{
	vector<CompCache> caches;
	for (unsigned long mask : m_em->ExtractMasks(componentMask)) {

		// iterate signatures that contain the required 'entitySignature'
		auto & fileIndex = m_fileIndexes[mask];

		for (unsigned long signature : fileIndex.IndexedKeys()) {
			if ((signature & entitySignature) == entitySignature) {
				// try to find the cache in the master component cache
				auto & masterCache = m_componentCache[mask];
				auto it = masterCache.find(signature);
				if (it == masterCache.end()) {
					auto && cache = LoadCache(fileIndex.Search(signature), signature, mask);
					// cache the CompCache
					masterCache.insert(std::make_pair(entitySignature, cache));
					caches.push_back(cache);
				}
				else {
					caches.push_back(it->second);
				}
			}
		}
	}
	return caches;
}

void EntityRegion::SaveCaches()
{
	for (auto & componentMap : m_componentCache) {
		for (auto & signatureMap : componentMap.second) {
			SaveCache(signatureMap.second, signatureMap.first, componentMap.first);
		}
	}
}

CompCache EntityRegion::LoadCache(string && data, unsigned long entitySignature, unsigned long componentMask)
{
	std::istringstream stream(data);
	CompCache cache = std::make_shared<vector<shared_ptr<Components::Component>>>();
	while (!stream.eof()) {
		auto component = m_em->GetPrototype(componentMask);
		unsigned int id = 0;
		stream.read((char *)&id, sizeof(unsigned int));
		component->ID = id;
		component->Import(stream);
		EntityPtr entity;
		auto it = m_entities.find(id);
		if (it == m_entities.end()) {
			entity = EntityPtr(new Entity(id, entitySignature, m_em));
			// cache the entity
			m_entities.insert(std::make_pair(id, entity));
		} else {
			entity = it->second;
		}
		component->Entity = entity.get();
		entity->AddComponent(component);
		cache->push_back(component);
		
	}
	return cache;
}

string EntityRegion::SaveCache(CompCache & cache, unsigned long entitySignature, unsigned long componentMask)
{
	std::ostringstream stream;
	for (auto & component : (*cache)) {
		stream.write((const char *)&(component->ID), sizeof(unsigned int));
		component->Export(stream);
	}
	return stream.str();
}
