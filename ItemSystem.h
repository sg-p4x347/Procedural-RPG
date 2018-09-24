#pragma once
#include "WorldSystem.h"
#include "Inventory.h"
#include "WorldDomain.h"
namespace world {
	class ItemSystem :
		public WorldSystem
	{
	public:
		ItemSystem(WEM * entityManager, vector<string> & components, unsigned short updatePeriod);
		//----------------------------------------------------------------
		// Queries
		Inventory & GetPlayerInventory();
		Inventory & GetInventoryOf(EntityID entity);
		EntityPtr GetOpenContainer();
		EntityPtr GetPlayer();

		EntityPtr TypeOf(InventoryItem & item);
		//----------------------------------------------------------------
		// Modifiers
		void AddItem(Inventory & inventory, string itemType, int quantity);
		void AddItem(Inventory & inventory, InventoryItem item);
		//----------------------------------------------------------------
		// Categorization
		std::unordered_set<string> GetItemCatagories();
		vector<InventoryItem> ItemsInCategory(Inventory & inventory, string category);
		//----------------------------------------------------------------
		// Entity Factories
		EntityPtr NewContainer(Vector3 position, Vector3 rotation, string model);
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
}
