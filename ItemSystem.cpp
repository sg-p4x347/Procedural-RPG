#include "pch.h"
#include "ItemSystem.h"
#include "IEventManager.h"
#include "Inventory.h"
#include "Item.h"
#include "AssetManager.h"
ItemSystem::ItemSystem(unique_ptr<WorldEntityManager> & entityManager, vector<string> & components, unsigned short updatePeriod) :
	WorldSystem::WorldSystem(entityManager,components,updatePeriod)
{
	RegisterHandlers();
	InitializeItemCatagories();
}

std::unordered_set<string> ItemSystem::GetItemCatagories()
{
	std::unordered_set<string> catagories;
	for (auto & catagory : m_itemCatagories) {
		catagories.insert(catagory.first);
	}
	return catagories;
}

shared_ptr<Components::Inventory> ItemSystem::GetPlayerInventory()
{
	return EM->Player()->GetComponent<Components::Inventory>("Inventory");
}

EntityPtr ItemSystem::TypeOf(Components::InventoryItem & item)
{
	EntityPtr type;
	AssetManager::Get()->GetStaticEM()->Find(item.TypeEntity, type);
	return type;
}

vector<Components::InventoryItem> ItemSystem::ItemsInCategory(shared_ptr<Components::Inventory> inventory, string category)
{
	vector<Components::InventoryItem> items;
	for (auto & item : inventory->Items) {
		if (m_itemCatagories[category].find(item.TypeEntity) != m_itemCatagories[category].end()) {
			items.push_back(item);
		}
	}
	return items;
}

void ItemSystem::RegisterHandlers()
{
	IEventManager::RegisterHandler(EventTypes::Item_OpenInventory, std::function<void(EntityPtr)>([this](EntityPtr entity) {
		OpenInventory(entity);
	}));
	IEventManager::RegisterHandler(EventTypes::Item_CloseInventory, std::function<void(EntityPtr)>([this](EntityPtr entity) {
		CloseInventory(entity);
	}));
}

void ItemSystem::OpenInventory(EntityPtr entity)
{
	if (entity) {
		shared_ptr<Components::Inventory> inventory;
		if(entity->TryGetComponent<Components::Inventory>("Inventory", inventory)) {
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
