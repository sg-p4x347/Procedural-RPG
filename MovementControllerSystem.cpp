#include "pch.h"
#include "MovementControllerSystem.h"
#include "IEventManager.h"
namespace world {


	MovementControllerSystem::MovementControllerSystem(world::WEM * entityManager) : WorldSystem::WorldSystem(entityManager,1),
		m_entities(entityManager->NewEntityCache<Collision,Movement,MovementController>())
	{
		RegisterHandlers();
	}

	MovementControllerSystem::~MovementControllerSystem()
	{
	}
	string MovementControllerSystem::Name()
	{
		return "MovementController";
	}
	void MovementControllerSystem::Update(double & elapsed)
	{
		
		for (auto & entity : m_entities) {
			auto & collision = entity.Get<Collision>();
			auto & movement = entity.Get<Movement>();
			auto & movementController = entity.Get<MovementController>();
			//----------------------------------------------------------------
			// Movement
			movement.Velocity.x = movementController.Heading.x * movementController.Speed;
			if (movementController.Type == MovementController::MovementTypes::Spectator)
				movement.Velocity.y = movementController.Heading.y * movementController.Speed;
			movement.Velocity.z = movementController.Heading.z * movementController.Speed;
			//----------------------------------------------------------------
			// Jump
			if (movementController.Type == MovementController::MovementTypes::Normal && movementController.Jump) {
				for (auto & contact : collision.Contacts) {
					// Jump if there is a collision normal facing up-ish to push off of
					if (std::abs(contact.contactNormal.Dot(Vector3::Up)) >= 0.5f) {
						movement.Velocity.y += 5.f;
						break;
					}
				}
			}
		}
	}
	void MovementControllerSystem::RegisterHandlers()
	{
		IEventManager::RegisterHandler(EventTypes::WEM_RegionLoaded, std::function<void(shared_ptr<WEM::RegionType>)>([=](shared_ptr<WEM::RegionType> region) {
			EntityCache<Collision, Movement, MovementController> cache;
			region->LoadEntities(cache);
			m_entities.GetCaches()[region] = cache;
		}));
		IEventManager::RegisterHandler(EventTypes::WEM_RegionUnloaded, std::function<void(shared_ptr<WEM::RegionType>)>([=](shared_ptr<WEM::RegionType> region) {
			m_entities.GetCaches().erase(region);
		}));
		IEventManager::RegisterHandler(EventTypes::Entity_SignatureChanged, std::function<void(shared_ptr<WEM::RegionType>, EntityID,MaskType)>([=](shared_ptr<WEM::RegionType> region, EntityID id, MaskType signature) {
			if (signature && m_entities.GetCaches().find(region) != m_entities.GetCaches().end()) {
				region->LoadEntities(m_entities.GetCaches()[region]);
			}
		}));
	}
}