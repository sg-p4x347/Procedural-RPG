#pragma once
#include "Component.h"
#include "EventManager.h"
namespace Components {
	class Action :
		public Component
	{
	public:
		Action();
		Action(Vector3 size, EventTypes event, unsigned int targetEntity);
		// Data
		Vector3 Size;
		EventTypes Event;
		unsigned int TargetEntity;
		// Inherited via Component
		virtual string GetName() override;

		void Import(ifstream & ifs) override;
		void Export(ofstream & ofs) override;
	};
}
