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
		bool EventBubbling;
		// Inherited via Component
		virtual string GetName() override;

		// Inherited via Component
		virtual shared_ptr<Components::Component> Copy(shared_ptr<Components::Component> source) override;
	};
}

