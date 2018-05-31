#include "pch.h"
#include "ResourceSystem.h"
#include "IEventManager.h"
#include "Inventory.h"
ResourceSystem::ResourceSystem(WorldEntityManager * entityManager, vector<string>& components, unsigned short updatePeriod)
	:WorldSystem::WorldSystem(entityManager,components,updatePeriod)
{
	RegisterHandlers();
}

string ResourceSystem::Name()
{
	return "Resource";
}

void ResourceSystem::Update(double & elapsed)
{
}

void ResourceSystem::Generate()
{

}

void ResourceSystem::RegisterHandlers()
{
	IEventManager::RegisterHandler(EventTypes::Resource_Aquire, std::function<void(EntityPtr,EntityPtr)>([=](EntityPtr actor,EntityPtr target) {
		shared_ptr<Components::Inventory> targetInventory = target->GetComponent<Components::Inventory>("Inventory");
		shared_ptr<Components::Inventory> actorInventory = actor->GetComponent<Components::Inventory>("Inventory");
		if (targetInventory && actorInventory) {
			// Begin the transaction by transferring the resource's inventory to the actor's inventory
			IEventManager::Invoke(EventTypes::Item_Transfer, targetInventory, actorInventory);
			// Delete the resource entity
			EM->DeleteEntity(target);
		}
		
	}));
}
