#pragma once
#include "ComponentCache.h"
#include "WorldEntityProxy.h"
namespace world {
	template<typename ... CompTypes>
	class Entity {
		typedef typename map<MaskType, tuple<shared_ptr<ComponentCache<CompTypes>>...>>::iterator iterator;
	public:

		Entity(iterator mapIterator, int index) : m_mapIterator(mapIterator), m_index(index) {

		}
		template<typename CompType>
		CompType & Get() {
			return get<shared_ptr<ComponentCache<CompType>>>(m_mapIterator->second)->Get()[m_index];
		}
		EntityID GetID() {
			return get<0>(m_mapIterator->second)->Get()[m_index].ID;
		}
		MaskType GetSignature() {
			return m_mapIterator->first;
		}
		WorldEntityProxy<CompTypes...> GetProxy() {
			return WorldEntityProxy<CompTypes...>(m_mapIterator, m_index);
		}
		iterator m_mapIterator;
		int m_index;
		
	};
}
