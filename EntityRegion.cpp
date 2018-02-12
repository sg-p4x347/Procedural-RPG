#include "pch.h"
#include "EntityRegion.h"

namespace Components {

	EntityRegion::EntityRegion()
	{
	}
	string EntityRegion::GetName()
	{
		return "EntityRegion";
	}
	void EntityRegion::Export(ofstream & ofs)
	{
		for (unsigned int & entity : Entities) {
			Serialize(entity, ofs);
		}
	}
	void EntityRegion::Import(ifstream & ifs)
	{
		while (!ifs.eof()) {
			unsigned int entity = 0;
			DeSerialize(entity, ifs);
			Entities.push_back(entity);
		}
	}
}
