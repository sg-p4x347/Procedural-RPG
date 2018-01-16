#include "pch.h"
#include "GuiEntityManager.h"
#include "GuiPanel.h"
#include "GuiStyle.h"
#include "GuiChildren.h"
#include "Sprite.h"
#include "GuiEventHandler.h"
#include "GuiText.h"
#include "Tag.h"
namespace GUI {
	GuiEntityManager::GuiEntityManager() : BaseEntityManager::BaseEntityManager()
	{
		//----------------------------------------------------------------
		// Initialize the component mappings
		AddComponentVector("Panel");
		AddComponentVector("Children");
		AddComponentVector("Sprite");
		AddComponentVector("Text");
		//----------------------------------------------------------------
		// Styles
		AddComponentVector("Style_Default");
		AddComponentVector("Style_Hover");
		AddComponentVector("Style_Active");
		//----------------------------------------------------------------
		// EventHanlders
		AddComponentVector("EventHandler_Click");
	}


	GuiEntityManager::~GuiEntityManager()
	{
	}

	EntityPtr GuiEntityManager::NewPanel(Style * style, vector<EntityPtr> children)
	{
		EntityPtr entity = NewEntity();
		entity->AddComponent(ComponentMask("Panel"), shared_ptr<Components::Component>(new Panel()));
		entity->AddComponent(ComponentMask("Style_Default"), shared_ptr<Components::Component>(style));
		entity->AddComponent(ComponentMask("Sprite"), std::shared_ptr<Components::Component>(new Sprite()));
		if (children.size() != 0) AddChildren(entity,children);
		return entity;
	}

	EntityPtr GuiEntityManager::NewButton(string text, std::function<void()>&&clickCallback, Style * style)
	{
		// Create World button
		EntityPtr btn = NewPanel([=] {
			if (!style) {
				Style* defaultStyle = new Style();
				defaultStyle->Background = "widget.png:button";
				defaultStyle->Height = "100px";
				return defaultStyle;
			}
			else {
				return style;
			}
		}());
		btn->AddComponent(ComponentMask("Text"), std::shared_ptr<Text>(new Text(text)));
		btn->AddComponent(ComponentMask("Style_Hover"), std::shared_ptr<Style>([] {
			Style * style = new Style();
			style->Background = "widget.png:button_hover";
			return style;
		}()));
		btn->AddComponent(ComponentMask("Style_Active"), std::shared_ptr<Style>([] {
			Style * style = new Style();
			style->Background = "widget.png:button_active";
			return style;
		}()));
		AddEventHandler(btn, new EventHandler("Click", std::move(clickCallback)));
		return btn;
	}

	void GuiEntityManager::AddChildren(EntityPtr entity, vector<EntityPtr> children)
	{
		if (entity) {
			vector<unsigned int> ids;
			for (auto & entity : children) {
				ids.push_back(entity->ID());
			}
			if (entity->HasComponents(ComponentMask("Children"))) {
				// Add children to existing component
				shared_ptr<Children> comp = GetComponent<Children>(entity,"Children");
				comp->Entities.insert(comp->Entities.end(), ids.begin(), ids.end());
			}
			else {
				// Add new children component
				entity->AddComponent(ComponentMask("Children"), shared_ptr<Components::Component>(
					new Children(ids)
				));
			}
		}
	}

	void GuiEntityManager::AddEventHandler(EntityPtr entity, EventHandler * handler)
	{
		if (entity && handler) {
			entity->AddComponent(ComponentMask(handler->GetName()), shared_ptr<Components::Component>(handler));
		}
	}

	void GuiEntityManager::AddText(EntityPtr entity, string text)
	{
		if (entity && text != "") {
			entity->AddComponent(ComponentMask("Text"), shared_ptr<Components::Component>(new Text(text)));
		}
	}

	shared_ptr<Components::Component> GuiEntityManager::InstantiateComponent(unsigned long & mask, EntityPtr entity)
	{
		shared_ptr<Components::Component> component;
		std::size_t underscorePos = m_names[mask].find('_');
		if (underscorePos == string::npos) {
			// Discrete component types
			if (m_names[mask] == "Panel") {
				component = shared_ptr<Components::Component>(new Panel(entity->ID()));
			}  else if (m_names[mask] == "Children") {
				component = shared_ptr<Components::Component>(new Children(entity->ID()));
			}
			else if (m_names[mask] == "Sprite") {
				component = shared_ptr<Components::Component>(new Sprite(entity->ID()));
			}
		}
		else {
			// Deferred types
			string discreteType = m_names[mask].substr(0, underscorePos);	// before the '_'
			string deferredType = m_names[mask].substr(underscorePos + 1);	// after the '_'
			if (discreteType == "Tag") {
				component = shared_ptr<Components::Component>(new Components::Tag(entity->ID(), deferredType));
			} else if (m_names[mask] == "Style") {
				component = shared_ptr<Components::Component>(new Style(entity->ID(),deferredType));
			}
			else if (m_names[mask] == "EventHandler") {
				component = shared_ptr<Components::Component>(new EventHandler(entity->ID(), deferredType));
			}
		}
		return component;
	}
}
