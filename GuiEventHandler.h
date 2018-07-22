#pragma once
#include "Delegate.h"
namespace GUI {
	
	struct Event {
		Event(EntityPtr sender, Vector2 mousePos,int scrollDelta,char character,Keyboard::Keys key) {
			Sender = sender;
			MousePosition = mousePos;
			ScrollDelta = scrollDelta;
			Character = character;
			Key = key;
		}
		EntityPtr Sender;
		Vector2 MousePosition;
		int ScrollDelta;
		char Character;
		Keyboard::Keys Key;
	};
	typedef std::function<void(Event)> GuiEventHandlerSignature;
	class EventHandler :
		public Components::Delegate
	{
	public:
		EventHandler(string type);
		EventHandler(string type, GuiEventHandlerSignature &&callback);
		~EventHandler();
		// Data
		std::function<void(Event evt)> Callback;

		// Inherited via Delegate
		virtual string GetDiscreteName() override;

		// Inherited via Delegate
		virtual shared_ptr<Components::Component> Copy(shared_ptr<Components::Component> source) override;
	};
}

