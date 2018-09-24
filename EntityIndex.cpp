#include "pch.h"
#include "EntityIndex.h"
#include "EntityTypedefs.h"
namespace world {
	const string EntityIndex::m_fileName = "entity.index";
	const string EntityIndex::m_deletedFileName = "deleted.index";
	EntityIndex::EntityIndex(Filesystem::path directory) : m_directory(directory)
	{
		Load();
	}

	EntityIndex::~EntityIndex()
	{
		Save();
	}

	void EntityIndex::Delete(EntityID & id)
	{
		m_deletedIndex.push_back(id);
		EntityInfo * info;
		if (Find(id, info)) {
			info->signature = 0;
		}
	}

	void EntityIndex::Load()
	{
		// entity index
		{
			std::ifstream ifs(m_directory / m_fileName, std::ios::binary);
			if (ifs.is_open()) {
				int count = 0;
				ifs.read((char *)&count, sizeof(int));
				m_index.resize(count);
				if (count) ifs.read((char *)&m_index[0], sizeof(EntityInfo) * count);
			}
		}
		// deleted index
		{
			std::ifstream ifs(m_directory / m_deletedFileName, std::ios::binary);
			if (ifs.is_open()) {
				int count = 0;
				ifs.read((char *)&count, sizeof(int));
				m_deletedIndex.resize(count);
				if (count) ifs.read((char *)&m_deletedIndex[0], sizeof(EntityID) * count);
			}
		}
	}

	void EntityIndex::Save()
	{
		// entity index
		{
			std::ofstream ofs(m_directory / m_fileName, std::ios::binary);
			if (ofs.is_open()) {
				int count = m_index.size();
				ofs.write((char *)&count, sizeof(int));
				if (count > 0)
					ofs.write((char *)&m_index[0], sizeof(EntityInfo) * count);
			}
		}
		// deleted index
		{
			std::ofstream ofs(m_directory / m_deletedFileName, std::ios::binary);
			if (ofs.is_open()) {
				int count = m_deletedIndex.size();
				ofs.write((char *)&count, sizeof(int));
				if (count > 0)
					ofs.write((char *)&m_deletedIndex[0], sizeof(EntityID) * count);
			}
		}
	}

	bool EntityIndex::Find(EntityID & id, EntityInfo *& info)
	{
		if (m_index.size() > (id - 1)) {
			info = &m_index[id - 1];
			return info->signature != 0;
		}
		return false;
	}

	EntityID EntityIndex::New(EntityInfo && info)
	{
		EntityID id;
		// search for a deleted ID to use
		if (m_deletedIndex.size()) {
			// Use the last deleted ID
			id = m_deletedIndex.back();
			m_deletedIndex.pop_back();
		}
		else {
			// Claim the next unused ID
			id = m_index.size() + 1;
		}
		Add(id, info);
		return id;
	}

	void EntityIndex::Add(EntityID & id, EntityInfo & info)
	{
		if (m_index.size() <= id) {
			m_index.resize(id);
		}
		m_index[id - 1] = info;
	}
}