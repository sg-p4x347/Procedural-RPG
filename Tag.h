#pragma once
#include "Component.h"
namespace Components {
	class Tag :
		public Component
	{
	public:
		Tag(const unsigned int & id, string name);
		~Tag();

		string Name;
		// Inherited via Component
		virtual string GetName() override;
	};
}

