#pragma once
#include "Delegate.h"
namespace Components {
	class Tag :
		public Delegate
	{
	public:
		Tag(string type);

		// Inherited via Delegate
		virtual string GetDiscreteName() override;
	};
}

