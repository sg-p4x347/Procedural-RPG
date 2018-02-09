#pragma once
#include "Component.h"
namespace Components {
	class Action :
		public Component
	{
	public:
		Action(float radius = 1.f);
		// Data
		float Radius;
		// Inherited via Component
		virtual string GetName() override;
	};
}
