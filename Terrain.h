#pragma once
#include "Component.h"

namespace Components {
	class Terrain :
		public Component
	{
	public:
		Terrain(const Terrain & other);
		Terrain(const unsigned int & id);
		
		// Inherited via Component
		virtual string GetName() override;
	};
}

