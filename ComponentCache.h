#pragma once
#include "MappedFile.h"
#include "EntityTypedefs.h"
namespace world {
	template<typename CompType>
	class ComponentCache
	{
	public:
		ComponentCache(MaskType signature, MappedFile<MaskType> & file) : m_signature(signature), m_file(file) {

		}
		~ComponentCache() {
			// Save
			Save();
		}
		void Insert(CompType & component) {
			m_cache.push_back(component);
		}
		void Remove(CompType & component) {
			m_cache.erase(component);
		}
		vector<CompType> & Get() {
			return m_cache;
		}
		bool Get(EntityID id, CompType & component) {
			for (auto & comp : m_cache) {
				if (comp.ID == id) {
					component = comp;
					return true;
				}
			}
			return false;
		}
		void Save() {
			const char * dataPtr = (m_cache.size() > 0) ? (const char*)&m_cache[0] : nullptr;
			size_t size = sizeof(CompType) * m_cache.size();
			m_file.Insert(m_signature, dataPtr, size);
		}
		void Import() {
			auto data = m_file.Search(m_signature);
			if (data.size() > 0) {
				unsigned int count = data.size() / sizeof(CompType);
				auto compPtr = (CompType*)&data[0];
				m_cache = vector<CompType>(compPtr, compPtr + count);
			}
		}
		MaskType GetSignature() {
			return m_signature;
		}
	private:
		MaskType m_signature;
		MappedFile<MaskType> & m_file;
		vector<CompType> m_cache;
	};
}

