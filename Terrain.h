#pragma once
#include "Component.h"

namespace Components {
	class Terrain :
		public Component
	{
	public:
		Terrain(const Terrain & other);
		Terrain();
		
		// Inherited via Component
		virtual string GetName() override;
	};
}

