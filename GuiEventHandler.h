#pragma once
#include "Component.h"
namespace GUI {
	class EventHandler :
		public Components::Component
	{
	public:
		EventHandler(const unsigned int & id,string type);
		EventHandler(string type, std::function<void()>&&callback);
		~EventHandler();
		// Data
		string Type;
		std::function<void()> Callback;
		// Inherited via Component
		virtual string GetName() override;
	};
}

