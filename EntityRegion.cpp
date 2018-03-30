#include "pch.h"
#include "EntityRegion.h"

namespace Components {
	const int EntityRegion::m_sectorDivisions = 16;
	EntityRegion::EntityRegion(int regionWidth) : RegionWidth(regionWidth), m_sectors(m_sectorDivisions)
	{
	}
	string EntityRegion::GetName()
	{
		return "EntityRegion";
	}

	void EntityRegion::AddEntity(EntityPtr entity) {
		Entities.push_back(entity->ID());
		//
	}
	vector<unsigned int> EntityRegion::FindEntities(Vector3 center, float range)
	{
	}
	void EntityRegion::MoveEntity(EntityPtr entity, Vector3 lastPos)
	{
		
	}
	void EntityRegion::Export(ofstream & ofs)
	{
		Serialize(RegionWidth, ofs);
		for (unsigned int & entity : Entities) {
			Serialize(entity, ofs);
		}

	}
	void EntityRegion::Import(ifstream & ifs)
	{
		DeSerialize(RegionWidth, ifs);
		while (!ifs.eof()) {
			unsigned int entity = 0;
			DeSerialize(entity, ifs);
			Entities.push_back(entity);
		}
	}
}
