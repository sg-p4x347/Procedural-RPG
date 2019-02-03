#pragma once
#include "ComponentCache.h"
#include "WorldEntity.h"
namespace world {
	template<typename ... CompTypes>
	class EntityIterator;

	template<typename ... CompTypes>
	class EntityCache
	{

		friend class EntityIterator<CompTypes...>;
		friend class Entity<CompTypes...>;

		typedef EntityIterator<CompTypes...> iterator;
		typedef map<MaskType, tuple<shared_ptr<ComponentCache<CompTypes>>...>> SignatureMap;

	public:
		EntityCache() {}
		~EntityCache() {}
		template<typename CompType>
		void Insert(MaskType signature, shared_ptr<ComponentCache<CompType>> cache) {
			auto it = m_cache.find(signature);
			if (it == m_cache.end()) {
				tuple<shared_ptr<ComponentCache<CompTypes>>...> tuple;
				std::get<shared_ptr<ComponentCache<CompType>>>(tuple) = cache;
				m_cache.insert(make_pair(signature, tuple));
			}
			else {
				std::get<shared_ptr<ComponentCache<CompType>>>(it->second) = cache;
			}

		}
		tuple<shared_ptr<ComponentCache<CompTypes>>...> & CachesFor(MaskType signature) {
			return m_cache[signature];
		}
		bool HasCache(MaskType signature) {
			auto it = m_cache.find(signature);
			return it != m_cache.end();
		}
		iterator begin() {
			return iterator(this, 0, GetSize());
		}
		iterator end() {
			int size = GetSize();
			return iterator(this, size, size);
		}
		size_t GetSize() {
			size_t size = 0;
			for (auto & sigCache : m_cache) {
				size += get<0>(sigCache.second)->Get().size();
			}
			return size;
		}
		void Clear() {
			m_cache.clear();
		}
	private:
		SignatureMap m_cache;
	};

	template<typename ... CompTypes>
	class EntityIterator {
		typedef Entity<CompTypes...> EntityType;
		typedef EntityCache<CompTypes...> CacheType;
		typedef EntityIterator<CompTypes...> iterator;
		typedef typename map<MaskType, tuple<shared_ptr<ComponentCache<CompTypes>>...>>::iterator SignatureIterator;
	public:
		EntityIterator(CacheType * entityCache, int offset, int size) : m_entityCache(entityCache), m_offset(offset), m_size(size), m_entity(entityCache->m_cache.begin(), 0) {
			Update();
		}
		bool operator==(const iterator & other) {
			return m_entityCache == other.m_entityCache && m_offset == other.m_offset;
		}
		bool operator!=(const iterator & other) {
			return !(*this == other);
		}
		Entity<CompTypes...> & operator*() {
			return m_entity;
		}
		iterator & operator++() {
			++m_offset;
			m_entity.m_index++;
			Update();
			return *this;
		}
		iterator operator++(int) {
			EntityIterator<CompTypes...> clone(*this);
			++(*this);
			return clone;
		}
		void Update() {
			while (m_entity.m_mapIterator != m_entityCache->m_cache.end() && m_entity.m_index >= get<0>(m_entity.m_mapIterator->second)->Get().size()) {
				m_entity.m_index = 0;
				++(m_entity.m_mapIterator);
			}
		}
	private:
		CacheType * m_entityCache;
		EntityType m_entity;
		int m_offset;
		int m_size;
	};
}

