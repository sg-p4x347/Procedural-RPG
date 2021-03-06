#include "pch.h"
#include "ItemSystem.h"
#include "IEventManager.h"
#include "Inventory.h"
#include "Item.h"
#include "AssetManager.h"
namespace world {
	ItemSystem::ItemSystem(WEM * entityManager, vector<string> & components, unsigned short updatePeriod) :
		WorldSystem::WorldSystem(entityManager, components, updatePeriod)
	{
		RegisterHandlers();
		InitializeItemCatagories();
	}

	std::unordered_set<string> ItemSystem::GetItemCatagories()
	{
		std::unordered_set<string> catagories;
		catagories.insert("All");
		for (auto & catagory : m_itemCatagories) {
			catagories.insert(catagory.first);
		}
		return catagories;
	}

	Inventory & ItemSystem::GetPlayerInventory()
	{
		return GetInventoryOf(EM->PlayerID());
	}

	Inventory & ItemSystem::GetInventoryOf(EntityID entity)
	{
		return EM->GetEntity(entity)->Get<Inventory>();
	}

	EntityPtr ItemSystem::GetOpenContainer()
	{
		EntityPtr container;
		EM->Find(EM->Player()->GetComponent<Components::Player>("Player")->OpenContainer, container);
		return container;
	}

	EntityPtr ItemSystem::GetPlayer()
	{
		return EM->Player();
	}

	EntityPtr ItemSystem::TypeOf(Components::InventoryItem & item)
	{
		EntityPtr type;
		AssetManager::Get()->GetStaticEM()->Find(item.TypeEntity, type);
		return type;
	}

	void ItemSystem::AddItem(shared_ptr<Components::Inventory> inventory, string itemType, int quantity)
	{
		EntityPtr item = FindItem(itemType);
		for (auto & invItem : inventory->Items) {
			if (!invItem.Procedural && invItem.TypeEntity == item->ID()) {
				// combine with existing InventoryItem
				invItem.Quantity += quantity;
				return;
			}
		}
		// add a new item
		inventory->Items.push_back(Components::InventoryItem(item->ID(), false, quantity));
	}

	void ItemSystem::AddItem(shared_ptr<Components::Inventory> inventory, Components::InventoryItem item)
	{
		for (auto & invItem : inventory->Items) {
			if (!invItem.Procedural && invItem.TypeEntity == item.TypeEntity) {
				// combine with existing InventoryItem
				invItem.Quantity += item.Quantity;
				return;
			}
		}
		// add a new item
		inventory->Items.push_back(item);
	}

	vector<Components::InventoryItem> ItemSystem::ItemsInCategory(shared_ptr<Components::Inventory> inventory, string category)
	{
		vector<Components::InventoryItem> items;
		for (auto & item : inventory->Items) {
			if (category == "All" || m_itemCatagories[category].find(item.TypeEntity) != m_itemCatagories[category].end()) {
				items.push_back(item);
			}
		}
		return items;
	}

	EntityPtr ItemSystem::NewContainer(Vector3 position, Vector3 rotation, string model)
	{
		EntityPtr entity = EM->NewEntity();

		entity->AddComponent(
			new Components::Position(position, rotation));
		entity->AddComponent(
			new Components::Model(model, AssetType::Authored));
		entity->AddComponent(
			new Components::Inventory());
		entity->AddComponent(
			new Components::Action(Vector3(3.f, 3.f, 3.f), EventTypes::Item_OpenInventory, entity->ID()));
		return entity;
	}

	void ItemSystem::RegisterHandlers()
	{
		IEventManager::RegisterHandler(EventTypes::Item_OpenInventory, std::function<void(EntityPtr, EntityPtr)>([this](EntityPtr actor, EntityPtr target) {
			if (target && actor) {
				OpenInventory(target);

			}
		}));
		IEventManager::RegisterHandler(EventTypes::Item_CloseInventory, std::function<void(EntityPtr, EntityPtr)>([this](EntityPtr actor, EntityPtr target) {
			if (target && actor) {
				CloseInventory(target);
			}
		}));
		IEventManager::RegisterHandler(EventTypes::Item_Transfer, std::function<void(
			shared_ptr<Components::Inventory>,
			shared_ptr<Components::Inventory>)
		>([this](shared_ptr<Components::Inventory> source, shared_ptr<Components::Inventory> target) {
			// transfer the items
			for (auto & item : source->Items) {
				AddItem(target, item);
			}
			// clear the source inventory;
			source->Items.clear();
		}));
	}

	void ItemSystem::OpenInventory(EntityPtr entity)
	{
		if (entity) {
			shared_ptr<Components::Inventory> inventory;
			if (entity->TryGetComponent<Components::Inventory>("Inventory", inventory)) {
				inventory->Open = true;
			}
		}
	}

	void ItemSystem::CloseInventory(EntityPtr entity)
	{
		if (entity) {
			shared_ptr<Components::Inventory> inventory;
			if (entity->TryGetComponent<Components::Inventory>("Inventory", inventory)) {
				inventory->Open = false;
			}
		}
	}

	string ItemSystem::Name()
	{
		return "Item";
	}

	void ItemSystem::Update(double & elapsed)
	{
	}

	void ItemSystem::InitializeItemCatagories()
	{
		//----------------------------------------------------------------
		// Resources
		unordered_set<unsigned int> resources;
		resources.insert(FindItem("Logs")->ID());
		resources.insert(FindItem("Coal")->ID());
		resources.insert(FindItem("Charcoal")->ID());
		resources.insert(FindItem("Iron Ore")->ID());
		resources.insert(FindItem("Cut Limestone")->ID());
		m_itemCatagories.insert(std::make_pair("Resources", resources));
		//----------------------------------------------------------------
		// Containers
		unordered_set<unsigned int> containers;
		containers.insert(FindItem("Basket")->ID());
		containers.insert(FindItem("Clay Pot")->ID());
		m_itemCatagories.insert(std::make_pair("Containers", containers));
		//----------------------------------------------------------------
		// Misc
		unordered_set<unsigned int> misc;
		for (EntityPtr & entity : AssetManager::Get()->GetStaticEM()->FindEntities(AssetManager::Get()->GetStaticEM()->ComponentMask("Item"))) {
			bool hasCategory = false;
			for (auto & category : m_itemCatagories) {
				if (category.second.count(entity->ID()) != 0) {
					hasCategory = true;
					break;
				}
			}
			if (hasCategory) continue;
			// Has no defined category
			misc.insert(entity->ID());
		}
		m_itemCatagories.insert(std::make_pair("Misc", misc));
	}

	EntityPtr ItemSystem::FindItem(string name)
	{
		// Search cache
		if (m_items.find(name) != m_items.end())
			return m_items[name];
		for (auto & entity : AssetManager::Get()->GetStaticEM()->FindEntities(AssetManager::Get()->GetStaticEM()->ComponentMask("Item"))) {
			if (entity->GetComponent<Item>("Item")->Name == name) {
				m_items.insert(std::make_pair(name, entity));
				return entity;
			}
		}
		return nullptr;
	}

	bool ItemSystem::TryFindItem(string name, EntityPtr & entity)
	{
		entity = FindItem(name);
		return entity != nullptr;
	}
}