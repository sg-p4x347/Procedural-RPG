#include "pch.h"
#include "InventoryGuiHandler.h"
#include "SystemManager.h"
#include "ItemSystem.h"
#include "Item.h"

InventoryGuiHandler::InventoryGuiHandler(SystemManager * sm, GUI::GuiEntityManager & em) : GuiHandler::GuiHandler(sm,em)
{
}

string InventoryGuiHandler::Name()
{
	return "InventoryGui";
}

void InventoryGuiHandler::RegisterHandlers()
{
	RegisterEventHandler("click", [&](Event evt) {
		m_guiSystem->SetHandMenu(EM.Copy(evt.Sender));
	});
}

EntityPtr InventoryGuiHandler::GetElement()
{
	EntityPtr inv = m_guiSystem->ImportMarkup("UI/inv.xml");
	EntityPtr header = m_guiSystem->FindElementByIdRecursive(inv, "header");
	EntityPtr body = m_guiSystem->FindElementByIdRecursive(inv, "body");

	vector<EntityPtr> tabs;
	EntityPtr tabTemplate = m_guiSystem->ImportMarkup("UI/inv_tab.xml");


	static string currentCategory = "All";
	auto categories = SM->GetSystem<ItemSystem>("Item")->GetItemCatagories();
	categories.insert("craft");
	for (string category : categories) {
		tabs.push_back([=] {
			EntityPtr tab = EM.Copy(tabTemplate);
			EM.AddText(tab, category);
			EM.AddEventHandler(tab, new GUI::EventHandler("Click", [=](GUI::Event evt) {
				currentCategory = category;
				SelectInventoryTab(body, category);
			}));
			return tab;
		}());
	}

	// Initialize the current tab
	SelectInventoryTab(body, currentCategory);
	// Add the tabs to the header
	EM.AddChildren(header, tabs);
	return inv;
}
EntityPtr InventoryGuiHandler::CreateInventoryGrid(string gridTemplate, vector<Components::InventoryItem> items)
{
	auto invGrid = m_guiSystem->ImportMarkup(gridTemplate);
	auto cellTemplate = m_guiSystem->ImportMarkup("UI/inv_item.xml",this);
	vector<EntityPtr> rows;
	for (auto & item : items) {
		auto itemType = SM->GetSystem<ItemSystem>("Item")->TypeOf(item)->GetComponent<Item>("Item");
		// Add a grid cell
		auto cell = EM.Copy(cellTemplate);
		/*EM.AddEventHandler(cell, new GUI::EventHandler("Click", [=](Event evt) {
			m_guiSystem->SetHandMenu(EM.Copy(cell));
		}));*/
		// label
		auto label = m_guiSystem->FindElementByIdRecursive(cell, "label");
		if (label) EM.AddText(label, itemType->Name);

		EM.AddText(cell, to_string(item.Quantity));
		m_guiSystem->GetStyle(cell)->Background = itemType->Sprite;

		EM.AddChild(invGrid, cell);
	}
	return invGrid;
}

void InventoryGuiHandler::SelectInventoryTab(EntityPtr gridContainer, string category)
{
	auto & itemSystem = SM->GetSystem<ItemSystem>("Item");
	m_guiSystem->DeleteChildren(gridContainer);
	EntityPtr openContainer = itemSystem->GetOpenContainer();
	if (openContainer && openContainer != itemSystem->GetPlayer()) {
		EntityPtr playerGrid = CreateInventoryGrid(
			"UI/half_inv_grid.xml",
			itemSystem->ItemsInCategory(
				itemSystem->GetPlayerInventory(),
				category
			)
		);
		EM.AddChild(gridContainer, playerGrid);
		EM.AddChild(gridContainer, m_guiSystem->ImportMarkup("UI/inv_divider.xml"));
		EntityPtr containerGrid = CreateInventoryGrid(
			"UI/half_inv_grid.xml",
			itemSystem->ItemsInCategory(
				itemSystem->GetInventoryOf(openContainer),
				category
			)
		);
		EM.AddChild(gridContainer, containerGrid);
	}
	else {
		EntityPtr grid = CreateInventoryGrid(
			"UI/inv_grid.xml",
			SM->GetSystem<ItemSystem>("Item")->ItemsInCategory(
				SM->GetSystem<ItemSystem>("Item")->GetPlayerInventory(),
				category
			)
		);
		EM.AddChild(gridContainer, grid);
	}

	m_guiSystem->UpdateFlowRecursive(gridContainer, 1);
}
