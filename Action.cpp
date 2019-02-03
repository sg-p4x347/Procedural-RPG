#include "pch.h"
#include "Action.h"

namespace world {
	Action::Action()
	{

	}


	Action::Action(Vector3 size, EventTypes event, unsigned int targetEntity): Size(size), Event(event), TargetEntity(targetEntity)
	{
	}

	/*void Action::Import(std::istream & ifs)
	{
		DeSerialize(Size, ifs);
		DeSerialize(Event, ifs);
		DeSerialize(TargetEntity, ifs);
	}
	void Action::Export(std::ostream & ofs)
	{
		Serialize(Size, ofs);
		Serialize(Event, ofs);
		Serialize(TargetEntity, ofs);
	}*/
}