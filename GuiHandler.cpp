#include "pch.h"
#include "GuiHandler.h"
#include "SystemManager.h"

namespace GUI {
	GuiHandler::GuiHandler(SystemManager * sm, GUI::GuiEntityManager & em) : System::System(0), SM(sm), EM(em)
	{
		m_guiSystem = SM->GetSystem<GuiSystem>("Gui");
	}
	void GuiHandler::Update(double & elapsed)
	{
	}
	void GuiHandler::RegisterEventHandler(string key, GuiEventHandlerSignature handler)
	{
		if (m_eventHandlers.find(key) == m_eventHandlers.end()) {
			m_eventHandlers.insert(std::make_pair(key, vector<GuiEventHandlerSignature>()));
		}
		m_eventHandlers[key].push_back(handler);
	}
	void GuiHandler::InvokeEvent(string key,Event evt)
	{
		if (m_eventHandlers.find(key) != m_eventHandlers.end()) {
			for (auto & handler : m_eventHandlers[key]) {
				handler(evt);
			}
		}

	}
}
