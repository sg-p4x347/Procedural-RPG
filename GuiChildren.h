#pragma once
#include "Component.h"
namespace GUI {
	class Children :
		public Components::Component
	{
	public:
		Children();
		Children(vector<unsigned int> children);
		vector<unsigned int> Entities;
		~Children();

		// Inherited via Component
		virtual string GetName() override;

		// Inherited via Component
		virtual shared_ptr<Components::Component> Copy(shared_ptr<Components::Component> source) override;
	};
}

