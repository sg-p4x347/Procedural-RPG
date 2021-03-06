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

			//if (m_cache.size() > 0) {
				
				if (is_base_of<ISerialization, CompType>()) {
					ostringstream ss;
					size_t size = m_cache.size();
					ss.write((const char *)&size,sizeof(size_t));
					for (auto & component : m_cache) {
						((ISerialization *)&component)->Export(ss);
					}
					m_file.Insert(m_signature, ss.str());
					m_file.Save();
				}
				else {
					const char * dataPtr = m_cache.size() ? (const char*)&m_cache[0] : nullptr;
					size_t size = sizeof(CompType) * m_cache.size();
					m_file.Insert(m_signature, dataPtr, size);
					
				}
				
				m_file.Save();
			//}
		}
		void Import() {
			size_t size = 0;
			char * data = nullptr;
			m_file.Search(m_signature, data, size);
			if (size > 0) {
				if (is_base_of<ISerialization,CompType>()) {
					// CompType is variable size, so stream each one in sequentially
					istringstream ss = istringstream(string(data, size));
					size_t size;
					ss.read((char *)&size, sizeof(size_t));
					for (int i = 0; i < size; i++) {
						CompType component{};
						((ISerialization *)&component)->Import(ss);
						if (!ss.eof()) {
							m_cache.push_back(component);
						}
						else {
							break;
						}
					}
				}
				else {
					// CompType is a fixed size, so just do an in-place vector initialization
					unsigned int count = size / sizeof(CompType);
					if (count == 0) count = 1;
					auto compPtr = (CompType*)data;
					m_cache = vector<CompType>(compPtr, compPtr + count);
				}
			}
			delete[] data;
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

