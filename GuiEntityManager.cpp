#include "pch.h"
#include "GuiEntityManager.h"
#include "GuiPanel.h"
#include "GuiStyle.h"
#include "GuiChildren.h"
#include "Sprite.h"
#include "GuiEventHandler.h"
#include "GuiText.h"
#include "Tag.h"
#include "Entity.h"
namespace GUI {
	GuiEntityManager::GuiEntityManager() : BaseEntityManager::BaseEntityManager()
	{
		//----------------------------------------------------------------
		// Initialize the component mappings
		RegisterComponent([] {return new Panel();});
		RegisterComponent([] {return new Children();});
		RegisterComponent([] {return new Panel();});
		RegisterComponent([] {return new Text();});
		//----------------------------------------------------------------
		// Styles
		RegisterDelegate([](string type) {return new Style(type);}, vector<string>{
			"Default",
			"Hover",
			"Active"
		});
		//----------------------------------------------------------------
		// EventHanlders
		RegisterDelegate([](string type) {return new EventHandler(type);}, vector<string>{
			"Click"
		});
	}


	GuiEntityManager::~GuiEntityManager()
	{
	}

	EntityPtr GuiEntityManager::NewPanel(Style * style, vector<EntityPtr> children)
	{
		EntityPtr entity = NewEntity();
		entity->AddComponent(new Panel());
		entity->AddComponent(style);
		entity->AddComponent(new Sprite());
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
		btn->AddComponent(new Text(text));
		btn->AddComponent([] {
			Style * style = new Style("Hover");
			style->Background = "widget.png:button_hover";
			return style;
		}());
		btn->AddComponent([] {
			Style * style = new Style("Active");
			style->Background = "widget.png:button_active";
			return style;
		}());
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
				shared_ptr<Children> comp = entity->GetComponent<Children>("Children");
				comp->Entities.insert(comp->Entities.end(), ids.begin(), ids.end());
			}
			else {
				// Add new children component
				entity->AddComponent(
					new Children(ids)
				);
			}
		}
	}

	void GuiEntityManager::AddEventHandler(EntityPtr entity, EventHandler * handler)
	{
		if (entity && handler) {
			entity->AddComponent(handler);
		}
	}

	void GuiEntityManager::AddText(EntityPtr entity, string text)
	{
		if (entity && text != "") {
			entity->AddComponent(new Text(text));
		}
	}

	//shared_ptr<Components::Component> GuiEntityManager::LoadComponent(unsigned long & mask, EntityPtr entity)
	//{
	//	shared_ptr<Components::Component> component;
	//	std::size_t underscorePos = m_names[mask].find('_');
	//	if (underscorePos == string::npos) {
	//		// Discrete component types
	//		if (m_names[mask] == "Panel") {
	//			component = shared_ptr<Components::Component>(new Panel(entity->ID()));
	//		}  else if (m_names[mask] == "Children") {
	//			component = shared_ptr<Components::Component>(new Children(entity->ID()));
	//		}
	//		else if (m_names[mask] == "Sprite") {
	//			component = shared_ptr<Components::Component>(new Sprite(entity->ID()));
	//		}
	//	}
	//	else {
	//		// Deferred types
	//		string discreteType = m_names[mask].substr(0, underscorePos);	// before the '_'
	//		string deferredType = m_names[mask].substr(underscorePos + 1);	// after the '_'
	//		if (discreteType == "Tag") {
	//			component = shared_ptr<Components::Component>(new Components::Tag(entity->ID(), deferredType));
	//		} else if (m_names[mask] == "Style") {
	//			component = shared_ptr<Components::Component>(new Style(entity->ID(),deferredType));
	//		}
	//		else if (m_names[mask] == "EventHandler") {
	//			component = shared_ptr<Components::Component>(new EventHandler(entity->ID(), deferredType));
	//		}
	//	}
	//	return component;
	//}
}
