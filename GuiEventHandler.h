#pragma once
#include "Delegate.h"
namespace GUI {
	class EventHandler :
		public Components::Delegate
	{
	public:
		EventHandler(string type);
		EventHandler(string type, std::function<void()>&&callback);
		~EventHandler();
		// Data
		std::function<void()> Callback;

		// Inherited via Delegate
		virtual string GetDiscreteName() override;
	};
}

