#pragma once
#include "Delegate.h"
namespace GUI {
	struct Event {
		Event(Vector2 mousePos,int scrollDelta,char character,Keyboard::Keys key) {
			MousePosition = mousePos;
			ScrollDelta = scrollDelta;
			Character = character;
			Key = key;
		}
		Vector2 MousePosition;
		int ScrollDelta;
		char Character;
		Keyboard::Keys Key;
	};
	class EventHandler :
		public Components::Delegate
	{
	public:
		EventHandler(string type);
		EventHandler(string type, std::function<void(Event evt)>&&callback);
		~EventHandler();
		// Data
		std::function<void(Event evt)> Callback;

		// Inherited via Delegate
		virtual string GetDiscreteName() override;
	};
}

