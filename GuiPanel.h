#pragma once
#include "Component.h"
namespace GUI {
	class Panel :
		public Components::Component
	{
	public:
		Panel();
		Panel(string id);
		~Panel();
		// Data
		unsigned int Parent;
		string ElementID;
		// Inherited via Component
		virtual string GetName() override;
	};
}

