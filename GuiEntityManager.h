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
		EntityPtr NewPanel(Style * style, vector<EntityPtr> children = vector<EntityPtr>());
		EntityPtr NewButton(string text, std::function<void()>&& clickCallback,Style * style = nullptr);
		//----------------------------------------------------------------
		// Entity modifiers
		void AddChildren(EntityPtr entity, vector<EntityPtr> children);
		void AddEventHandler(EntityPtr entity, EventHandler * handler);
		void AddText(EntityPtr entity, string text);
	protected:
		// Inherited via BaseEntityManager
		virtual shared_ptr<Components::Component> InstantiateComponent(unsigned long & mask, EntityPtr entity) override;
	};
}

