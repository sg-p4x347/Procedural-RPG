#pragma once
#include "WorldComponent.h"
#include "EventManager.h"
namespace world {
	class Action :
		public WorldComponent
	{
	public:
		Action();
		Action(Vector3 size, EventTypes event, unsigned int targetEntity);
		// Data
		Vector3 Size;
		EventTypes Event;
		unsigned int TargetEntity;
		// Inherited via Component

		//void Import(std::istream & ifs) override;
		//void Export(std::ostream & ofs) override;
	};
}
