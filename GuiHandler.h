#pragma once
#include "GuiEntityManager.h"
#include "System.h"
#include "GuiSystem.h"
#include "GuiEventHandler.h"

class SystemManager;
namespace GUI {
	class GuiHandler : public System
	{
	public:
		GuiHandler(SystemManager * sm, GUI::GuiEntityManager & em);
		virtual void RegisterHandlers() = 0;
		virtual EntityPtr GetElement() = 0;
		void InvokeEvent(string key, Event evt);
	protected:
		SystemManager * SM;
		shared_ptr<GuiSystem> m_guiSystem;
		GUI::GuiEntityManager & EM;

		// Inherited via System
		virtual void Update(double & elapsed) override;

		void RegisterEventHandler(string key, GuiEventHandlerSignature handler);
		
	private:
		std::map<string, vector<GuiEventHandlerSignature>> m_eventHandlers;
	};
}

