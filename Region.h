#pragma once
#include "ComponentCache.h"
#include "EntityCache.h"
#include "MappedFile.h"
namespace world {
	template<typename ... CompTypes>
	class Region
	{
	public:
		Region(Filesystem::path directory, tuple<pair<MaskType, CompTypes>...> & maskIndex) : m_directory(directory), m_maskIndex(maskIndex) {
			Filesystem::create_directory(directory);
			LoadFile<CompTypes...>();
		}
		template<typename CompType>
		inline vector<std::shared_ptr<ComponentCache<CompType>>> LoadComponents(MaskType signature) {
			vector<std::shared_ptr<ComponentCache<CompType>>> caches;
			for (auto & cacheMapping : std::get<std::map<MaskType, shared_ptr<ComponentCache<CompType>>>>(m_componentCache)) {
				if (cacheMapping.first & signature == signature) {
					caches.push_back(cacheMapping.second);
				}
			}
			return caches;
		}
		template<typename CacheType, typename HeadType>
		inline void LoadEntitiesRecursive(CacheType & entityCache, MaskType componentMask, bool exclusive = false) {
			// Iterate the cached entity signatures
			auto & componentMap = std::get<std::map<MaskType, shared_ptr<ComponentCache<HeadType>>>>(m_componentCache);
			for (auto & cacheMapping : componentMap) {
				if (!exclusive && (cacheMapping.first & componentMask) == componentMask || exclusive && cacheMapping.first == componentMask) {
					// Proceed to load each component cache for this signature
					LoadComponents<CacheType, HeadType>(cacheMapping.first, entityCache);
				}
			}
			// Iterate over entity signatures in the database
			MaskType headMask = std::get<pair<MaskType, HeadType>>(m_maskIndex).first;
			auto end = componentMap.end();
			for (auto & signature : m_files[headMask].IndexedKeys()) {
				if ((!exclusive && (signature & componentMask) == componentMask || exclusive && signature == componentMask) && componentMap.find(signature) == end) {
					// Proceed to load and cache each component cache for this signature from the database
					ImportComponents<CacheType, HeadType>(signature, headMask, entityCache);
				}
			}
			
		}
		template<typename HeadType, typename ... MaskTypes>
		inline void LoadEntities(EntityCache<HeadType, MaskTypes...> & entityCache, MaskType componentMask, bool exclusive = false) {
			LoadEntitiesRecursive<EntityCache<HeadType, MaskTypes...>, HeadType, MaskTypes...>(entityCache, componentMask, exclusive);
		}
		template<typename CacheType, typename HeadType, typename NextType, typename ... MaskTypes>
		inline void LoadEntitiesRecursive(CacheType & entityCache, MaskType componentMask, bool exclusive = false) {
			LoadEntitiesRecursive<CacheType, HeadType>(entityCache, componentMask, exclusive);
			LoadEntitiesRecursive<CacheType, NextType, MaskTypes...>(entityCache, componentMask, exclusive);
		}
		//----------------------------------------------------------------
		// Change Signature
		inline void ChangeSignature(EntityID & id, MaskType & oldSig, MaskType & newSig) {
			EntityCache<CompTypes...> oldCache;
			LoadEntities<CompTypes...>(oldCache, oldSig, true);
			EntityCache<CompTypes...> newCache;
			LoadEntities<CompTypes...>(newCache, newSig, true);
			ChangeSignature<tuple<shared_ptr<ComponentCache<CompTypes>>...>, CompTypes...>(oldCache.CachesFor(oldSig), newCache.CachesFor(newSig), id, oldSig, newSig);
		}

		//----------------------------------------------------------------
		// Load Components
		template<typename EntityCacheType, typename HeadType, typename ... MaskTypes>
		inline void LoadComponents(MaskType signature, EntityCacheType & cache) {
			cache.Insert(signature, std::get<std::map<MaskType, shared_ptr<ComponentCache<HeadType>>>>(m_componentCache)[signature]);
			LoadComponents<EntityCacheType, MaskTypes...>(signature, cache);
		}
		template<typename EntityCacheType>
		inline void LoadComponents(MaskType signature, EntityCacheType & cache) {}

		template<typename EntityCacheType, typename HeadType, typename ... MaskTypes>
		inline void ImportComponents(MaskType signature, MaskType componentMask, EntityCacheType & cache) {
			auto & componentMap = std::get<std::map<MaskType, shared_ptr<ComponentCache<HeadType>>>>(m_componentCache);
			auto compCache = std::make_shared<ComponentCache<HeadType>>(signature, m_files[componentMask]);
			compCache->Import();
			componentMap.insert(std::make_pair(signature, compCache));
			cache.Insert(signature, compCache);
			ImportComponents<EntityCacheType, MaskTypes...>(signature, componentMask, cache);

		}
		template<typename EntityCacheType>
		inline void ImportComponents(MaskType signature, MaskType componentMask, EntityCacheType & cache) {}

		template<typename CompType>
		inline void InsertComponent(CompType & component, MaskType signature) {
			EntityCache<CompType> entityCache;
			LoadEntities(entityCache, signature, true);
			if (entityCache.HasCache(signature)) {

				std::get<shared_ptr<ComponentCache<CompType>>>(entityCache.CachesFor(signature))->Insert(component);
			}
			else {
				auto & sigMap = std::get<std::map<MaskType, shared_ptr<ComponentCache<CompType>>>>(m_componentCache);
				auto cache = make_shared<ComponentCache<CompType>>(signature, m_files[std::get<pair<MaskType, CompType>>(m_maskIndex).first]);
				cache->Insert(component);
				sigMap.insert(make_pair(signature, cache));
			}

			/*auto & sigMap = std::get<std::map<MaskType, shared_ptr<ComponentCache<CompType>>>>(m_componentCache);
			auto it = sigMap.find(signature);
			if (it != sigMap.end()) {
				it->second->Insert(component);
			}
			else {
				auto cache = make_shared<ComponentCache<CompType>>(signature,m_files[std::get<pair<MaskType, CompType>>(m_maskIndex).first]);
				cache->Insert(component);
				sigMap.insert(make_pair(signature, cache));
			}*/
		}
		void Save() {
			Save<CompTypes...>();
		}
		void EmptyCache() {
			EmptyCache<CompTypes...>();
		}
	private:
		Filesystem::path m_directory;
		std::map<MaskType, MappedFile<MaskType>> m_files;
		tuple<pair<MaskType, CompTypes>...> & m_maskIndex;
		std::tuple<std::map<MaskType, shared_ptr<ComponentCache<CompTypes>>>...> m_componentCache;
	private:
		template <typename HeadType>
		MaskType GetMask() {
			return std::get<pair<MaskType, HeadType>>(m_maskIndex).first;
		}
		template <typename HeadType>
		void LoadFile(MaskType mask) {
			string fileName = (m_directory / std::to_string(mask)).string();
			m_files.insert(std::make_pair(mask, MappedFile<MaskType>(fileName + ".index", fileName + ".dat")));
		}
		template<typename HeadType, typename Next, typename ... MaskTypes>
		void LoadFile(MaskType mask = 1) {
			LoadFile<HeadType>(mask);
			LoadFile<Next, MaskTypes...>(mask * 2);
		}

		template <typename HeadType>
		void Save() {
			auto & map = std::get<std::map<MaskType, shared_ptr<ComponentCache<HeadType>>>>(m_componentCache);
			for (auto & cache : map) {
				cache.second->Save();
			}
		}
		template <typename HeadType, typename Next, typename ... MaskTypes>
		void Save() {
			Save<HeadType>();
			Save<Next, MaskTypes...>();
		}

		template <typename HeadType>
		void EmptyCache() {
			std::get<std::map<MaskType, shared_ptr<ComponentCache<HeadType>>>>(m_componentCache).clear();
		}
		template <typename HeadType, typename Next, typename ... MaskTypes>
		void EmptyCache() {
			EmptyCache<HeadType>();
			EmptyCache<Next, MaskTypes...>();
		}
		//----------------------------------------------------------------
		// Signature change
		template <typename EntityCacheType, typename HeadType>
		void ChangeSignature(EntityCacheType & oldCache, EntityCacheType & newCache, EntityID & id, MaskType & oldSig, MaskType & newSig) {
			auto compCache = std::get<shared_ptr<ComponentCache<HeadType>>>(oldCache);
			if (compCache) {
				// find the index within the cache
				auto & vec = compCache->Get();
				for (int i = 0; i < vec.size(); i++) {
					if (vec[i].ID == id) {
						// add to the new cache
						MaskType mask = GetMask<HeadType>();
						if ((mask & newSig) == mask)
							InsertComponent<HeadType>(vec[i], newSig);
						// remove from the old cache
						vec.erase(vec.begin() + i);
						break;
					}
				}

			}

		}
		template<typename EntityCacheType, typename HeadType, typename Next, typename ... MaskTypes>
		void ChangeSignature(EntityCacheType & oldCache, EntityCacheType & newCache, EntityID & id, MaskType & oldSig, MaskType & newSig) {
			ChangeSignature<EntityCacheType, HeadType>(oldCache, newCache, id, oldSig, newSig);
			ChangeSignature<EntityCacheType, Next, MaskTypes...>(oldCache, newCache, id, oldSig, newSig);
		}
	};

}