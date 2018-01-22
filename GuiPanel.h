#pragma once
#include "Component.h"
namespace GUI {
	class Panel :
		public Components::Component
	{
	public:
		Panel();
		~Panel();
		// Data
		// Inherited via Component
		virtual string GetName() override;
	};
}

