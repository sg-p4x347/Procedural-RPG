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
	};
}

