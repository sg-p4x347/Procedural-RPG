#pragma once
#include "EntityTypedefs.h"
namespace world {
	struct EntityInfo {
		EntityInfo(MaskType sig, uint8_t x, uint8_t z) : signature(sig), regionX(x), regionZ(z) {}
		EntityInfo() : EntityInfo::EntityInfo(0, 0, 0) {}
		MaskType signature;
		uint8_t regionX;
		uint8_t regionZ;
	};
	class EntityIndex
	{
	public:
		EntityIndex(Filesystem::path directory);
		bool Find(EntityID & id, EntityInfo *& info);
		EntityID New(EntityInfo && info);
		void Delete(EntityID & id);
		void Save();
		~EntityIndex();
	private:
		void Load();
		void Add(EntityID & id, EntityInfo & info);
	private:
		static const string m_fileName;
		static const string m_deletedFileName;
		Filesystem::path m_directory;
		vector<EntityInfo> m_index;
		deque<EntityID> m_deletedIndex;
	};
}

