#include "pch.h"
#include "EntityRegion.h"

namespace Components {
	EntityRegion::EntityRegion(int regionWidth) : RegionWidth(regionWidth)
	{
	}
	string EntityRegion::GetName()
	{
		return "EntityRegion";
	}

	void EntityRegion::AddEntity(unsigned int entity) {
		Entities.push_back(entity);
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
