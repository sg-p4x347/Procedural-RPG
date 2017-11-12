#pragma once
#include "Component.h"
namespace Components {
	class Road :
		public Component {
	public:
		Road(const unsigned int & id);
		~Road();
	private:


		// Inherited via Component
		virtual string GetName() override;

	};
}