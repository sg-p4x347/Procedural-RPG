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
		RegisterComponent([] {return new Sprite();});
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
			"Click",
			"Drag",
			"Scroll",
			"Keydown",
			"MouseUp"
		});
	}


	GuiEntityManager::~GuiEntityManager()
	{
	}

	EntityPtr GuiEntityManager::NewPanel(Style * style, vector<EntityPtr> children,string id)
	{
		EntityPtr entity = NewEntity();
		entity->AddComponent(new Panel(id));
		entity->AddComponent(style);
		entity->AddComponent(new Sprite());
		if (!children.empty()) AddChildren(entity,children);
		return entity;
	}

	EntityPtr GuiEntityManager::NewTextPanel(string text, Style * style, string id)
	{
		EntityPtr panel = NewPanel(style,std::vector<EntityPtr>(),id);
		panel->AddComponent(new Text(text));
		return panel;
	}

	EntityPtr GuiEntityManager::NewButton(string text, std::function<void(Event evt)>&&clickCallback,string defaultBackground,string hoverBackground,string activeBackground)
	{
		// Create World button
		EntityPtr btn = NewPanel([=] {
			Style* style = new Style();
			style->FontColor = "rgb(1,1,1)";
			style->Background = defaultBackground;
			style->Height = "100px";
			return style;
		}());
		btn->AddComponent(new Text(text));
		btn->AddComponent([=] {
			Style * style = new Style("Hover");
			style->Background = hoverBackground;
			return style;
		}());
		btn->AddComponent([=] {
			Style * style = new Style("Active");
			style->Background = activeBackground;
			return style;
		}());
		AddEventHandler(btn, new EventHandler("Click", std::move(clickCallback)));
		return btn;
	}

	EntityPtr GuiEntityManager::Copy(EntityPtr source)
	{
		EntityPtr copy = source->Copy();
		if (source->HasComponents(ComponentMask("Children"))) {
			vector<unsigned int> & copyChildren = copy->GetComponent<GUI::Children>("Children")->Entities;
			copyChildren.clear();
			for (auto & childID : source->GetComponent<GUI::Children>("Children")->Entities) {
				EntityPtr child;
				if (Find(childID, child)) {
					// copy childern recursively
					AddChild(copy,Copy(child));
					//copyChildren.push_back(Copy(child)->ID());
				}
			}
		}
		if (source->HasComponents(ComponentMask("Panel"))) {
			source->GetComponent<GUI::Panel>("Panel")->Parent = 0;
		}
		return copy;
	}

	void GuiEntityManager::AddChild(EntityPtr entity, EntityPtr child)
	{
		if (entity && child) {
			child->GetComponent<GUI::Panel>("Panel")->Parent = entity->ID();
			if (entity->HasComponents(ComponentMask("Children"))) {
				// Add children to existing component
				shared_ptr<Children> comp = entity->GetComponent<Children>("Children");
				comp->Entities.push_back(child->ID());
			}
			else {
				// Add new children component
				entity->AddComponent(
					new Children(vector<unsigned int>{child->ID()})
				);
			}
		}
	}

	void GuiEntityManager::AddChildren(EntityPtr entity, vector<EntityPtr> children)
	{
		if (entity) {
			vector<unsigned int> ids;
			for (auto & child : children) {
				child->GetComponent<GUI::Panel>("Panel")->Parent = entity->ID();
				ids.push_back(child->ID());
				shared_ptr<Panel> panel = child->GetComponent<Panel>("Panel");
				if (panel) panel->Parent = entity->ID();
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
		if (entity) {
			shared_ptr<GUI::Text> textComp = entity->GetComponent<GUI::Text>("Text");
			if (textComp) {
				textComp->String = text;
			}
			else {
				entity->AddComponent(new Text(text));
			}
		}
	}

	void GuiEntityManager::CollectGarbage()
	{
		for (auto & entity : UnreferencedEntities()) {
			DeleteEntity(entity);
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
