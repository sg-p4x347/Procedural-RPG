#include "pch.h"
#include "Action.h"

namespace Components {
	Action::Action()
	{

	}


	Action::Action(Vector3 size, EventTypes event, unsigned int targetEntity): Size(size), Event(event), TargetEntity(targetEntity)
	{
	}

	string Components::Action::GetName()
	{
		return "Action";
	}
	void Action::Import(ifstream & ifs)
	{
		DeSerialize(Size, ifs);
		DeSerialize(Event, ifs);
		DeSerialize(TargetEntity, ifs);
	}
	void Action::Export(ofstream & ofs)
	{
		Serialize(Size, ofs);
		Serialize(Event, ofs);
		Serialize(TargetEntity, ofs);
	}
}