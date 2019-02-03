#pragma once
#include "ComponentCache.h"
namespace world {
	template<typename ... CompTypes>
	class WorldEntityProxy
	{
		typedef typename map<MaskType, tuple<shared_ptr<ComponentCache<CompTypes>>...>>::iterator iterator;
	public:
		WorldEntityProxy(iterator mapIterator, int index) : m_index(index) {
			SetCache<CompTypes...>(mapIterator);
		}
		~WorldEntityProxy() {}
		template<typename CompType>
		CompType & Get() {
			return std::get<shared_ptr<ComponentCache<CompType>>>(m_compCaches)->Get()[m_index];
		}
		EntityID GetID() {
			return get<0>(m_compCaches)->Get()[m_index].ID;
		}
		MaskType GetSignature() {
			return get<0>(m_compCaches)->GetSignature();
		}
	private:
		std::tuple<shared_ptr<ComponentCache<CompTypes>>...> m_compCaches;
		int m_index;
	private:
		template<typename HeadType>
		void SetCache(iterator & mapIterator) {
			std::get<shared_ptr<ComponentCache<HeadType>>>(m_compCaches) = std::get<shared_ptr<ComponentCache<HeadType>>>(mapIterator->second);
		}
		template<typename HeadType, typename NextType, typename ... Rest>
		void SetCache(iterator & mapIterator) {
			SetCache<HeadType>(mapIterator);
			SetCache<NextType,Rest...>(mapIterator);
		}
	};
}
