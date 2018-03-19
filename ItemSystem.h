#pragma once
#include "WorldSystem.h"
#include "Inventory.h"
class ItemSystem :
	public WorldSystem
{
public:
	ItemSystem(unique_ptr<WorldEntityManager> & entityManager, vector<string> & components, unsigned short updatePeriod);
	//----------------------------------------------------------------
	// Queries
	shared_ptr<Components::Inventory> GetPlayerInventory();
	EntityPtr TypeOf(Components::InventoryItem & item);
	//----------------------------------------------------------------
	// Categorization
	std::unordered_set<string> GetItemCatagories();
	vector<Components::InventoryItem> ItemsInCategory(shared_ptr<Components::Inventory> inventory, string category);

private:
	//----------------------------------------------------------------
	// Helpers
	void RegisterHandlers();
	
	//----------------------------------------------------------------
	// Modifiers
	void OpenInventory(EntityPtr entity);
	void CloseInventory(EntityPtr entity);
	// Inherited via WorldSystem
	virtual string Name() override;
	virtual void Update(double & elapsed) override;
	//----------------------------------------------------------------
	// Item catagorization
	void InitializeItemCatagories();
	std::map<string, unordered_set<unsigned int>> m_itemCatagories;
	//----------------------------------------------------------------
	// Item queries
	EntityPtr FindItem(string name);
	bool TryFindItem(string name, EntityPtr & entity);
	map<string, EntityPtr> m_items;
};

