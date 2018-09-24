#pragma once
#include "EntityCache.h"
#include "Region.h"
namespace world {
	template<typename RegionType, typename ... CompTypes>
	class WorldEntityIterator;

	template<typename RegionType, typename ... CompTypes>
	class WorldEntityCache
	{
		friend class WorldEntityIterator<RegionType, CompTypes...>;
		typedef WorldEntityIterator<RegionType, CompTypes...> iterator;


		typedef map<shared_ptr<RegionType>, EntityCache<CompTypes...>> RegionEntityCaches;

	public:
		WorldEntityCache(MaskType componentMask) : m_componentMask(componentMask) {}
		~WorldEntityCache() {}
		MaskType GetComponentMask() {
			return m_componentMask;
		}
		RegionEntityCaches & GetCaches() {
			return m_regionCaches;
		}
		iterator begin() {
			return iterator(*this, 0);

		}
		iterator end() {
			return iterator(*this, GetSize());
		}
	private:
		MaskType m_componentMask;
		RegionEntityCaches m_regionCaches;
	private:
		int GetSize() {
			int size = 0;
			for (auto & regionCache : m_regionCaches) {
				size += regionCache.second.GetSize();
			}
			return size;
		}
	};


	template<typename RegionType, typename ... CompTypes>
	class WorldEntityIterator {
		typedef Entity<CompTypes...> EntityType;
		typedef WorldEntityCache<RegionType, CompTypes...> CacheType;
		typedef WorldEntityIterator<RegionType, CompTypes...> iterator;
		typedef typename std::map<shared_ptr<RegionType>, EntityCache<CompTypes...>>::iterator RegionIterator;
	public:
		WorldEntityIterator(CacheType & wec, int offset) : m_wec(wec), m_regionIterator(wec.m_regionCaches.begin()), m_offset(offset), m_entityIterator(nullptr) {
			if (m_regionIterator != wec.m_regionCaches.end()) {
				m_entityIterator = unique_ptr<EntityIterator<CompTypes...>>(new EntityIterator<CompTypes...>(m_regionIterator->second.begin()));
				UpdateIterator();
			}
		}

		bool operator==(const iterator & other) {
			return &m_wec == &(other.m_wec) && m_offset == other.m_offset;
		}
		bool operator!=(const iterator & other) {
			return !(*this == other);
		}
		EntityType & operator*() {
			return **m_entityIterator;
		}
		iterator & operator++() {
			++m_offset;
			++*m_entityIterator;
			UpdateIterator();
			return *this;
		}
		iterator operator++(int) {
			WorldEntityIterator<RegionType, CompTypes...> clone(*this);
			++(*this);
			return clone;
		}
	private:
		CacheType & m_wec;
		int m_offset;
		RegionIterator m_regionIterator;
		unique_ptr<EntityIterator<CompTypes...>> m_entityIterator;
	private:
		void UpdateIterator() {
			if (m_entityIterator && *m_entityIterator == m_regionIterator->second.end()) {
				do {
					++m_regionIterator;
				} while (m_regionIterator != m_wec.m_regionCaches.end() && m_regionIterator->second.GetSize() == 0);


				if (m_regionIterator != m_wec.m_regionCaches.end())
					m_entityIterator.reset(new EntityIterator<CompTypes...>(m_regionIterator->second.begin()));
			}
		}
	};

}