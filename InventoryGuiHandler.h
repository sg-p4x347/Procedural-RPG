#pragma once
#include "GuiHandler.h"
class InventoryGuiHandler : public GUI::GuiHandler
{
public:
	InventoryGuiHandler(SystemManager * sm, GUI::GuiEntityManager & em);

	// Inherited via GuiHandler
	virtual string Name() override;
	virtual void RegisterHandlers() override;
	virtual EntityPtr GetElement() override;

private:
	EntityPtr CreateInventoryGrid(string gridTemplate, vector<Components::InventoryItem> items);
	void SelectInventoryTab(EntityPtr gridContainer, string category);

};

