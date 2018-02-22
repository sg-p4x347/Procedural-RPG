#pragma once
#include "BaseEntityManager.h"
#include "GuiStyle.h"
#include "GuiEventHandler.h"
namespace GUI {
	class GuiEntityManager :
		public BaseEntityManager
	{
	public:
		GuiEntityManager();
		~GuiEntityManager();
		//----------------------------------------------------------------
		// Entity Factories
		EntityPtr NewPanel(Style * style, vector<EntityPtr> children = vector<EntityPtr>(),string id = "");
		EntityPtr NewTextPanel(string text, Style * style);
		EntityPtr NewButton(string text, std::function<void(Event evt)>&& clickCallback, 
			string defaultBackground = "widget.png:button", 
			string hoverBackground = "widget.png:button_hover", 
			string activeBackground = "widget.png:button_active");
		//----------------------------------------------------------------
		// Entity modifiers
		void AddChildren(EntityPtr entity, vector<EntityPtr> children);
		void AddEventHandler(EntityPtr entity, EventHandler * handler);
		void AddText(EntityPtr entity, string text);
	protected:
	};
}

