#pragma once
#include "Component.h"
namespace GUI {
	class Panel :
		public Components::Component
	{
	public:
		Panel(const unsigned int & id);
		Panel();
		~Panel();
		// Data
		// Inherited via Component
		virtual string GetName() override;
	};
}

