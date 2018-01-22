#pragma once
#include "Component.h"
namespace Components {
	class Delegate :
		public Component
	{
	public:
		Delegate(string delegateName);
		// Data
		string DelegateName;
		// Differentiates inherited delegate components
		virtual string GetDiscreteName() = 0;
		// Inherited via Component
		virtual string GetName() override;
	};
}

