#include "pch.h"
#include "MovementSystem.h"
#include "IEventManager.h"
#include "TaskManager.h"
#include "WorldDomain.h"
namespace world {
	MovementSystem::MovementSystem(
		WEM *  entityManager,
		unsigned short updatePeriod,
		shared_ptr<RenderSystem> renderSys
		) :

		WorldSystem::WorldSystem(entityManager, updatePeriod),
		m_renderSystem(renderSys),
		m_moveTracker(entityManager->GetRegionWidth()),
		m_entities(entityManager->NewEntityCache<Position,Movement>())
	{
		IEventManager::RegisterHandler(EventTypes::WEM_Resync, std::function<void(void)>([=]() {
			SyncEntities();
		}));
		/*IEventManager::RegisterHandler(Entity_ComponentAdded, std::function<void(unsigned int, unsigned long)>([=](unsigned int id, unsigned long mask) {
			EntityPtr target;
			if (EM->Find(id, target)) {
				if (mask == m_componentMask) {
					m_entities.push_back(target);
				}
			}
		}));*/
	}

	string MovementSystem::Name()
	{
		return "Movement";
	}

	void MovementSystem::Update(double & elapsed)
	{
		auto playerPos = EM->PlayerPos();
		//TaskManager::Get().Push(Task([=] {
		std::vector<std::pair<EntityID, Vector3>> regionChanges;
		for (auto & entity : m_entities) {
			auto & position = entity.Get<Position>();
			auto & movement = entity.Get<Movement>();
			movement.Velocity += movement.Acceleration * elapsed;

			MovementTracker tracker(EM->GetRegionWidth());
			tracker.Update(position.Pos);
			position.Pos += movement.Velocity * elapsed;
			
			if (tracker.Update(position.Pos)) {
				// region boundary crossed
				regionChanges.push_back(std::make_pair(entity.GetID(), position.Pos));
			}


			// Rotations
			position.Rot += movement.AngularVelocity * elapsed;
			position.Rot.x = std::fmod(position.Rot.x, XM_2PI);
			position.Rot.y = std::fmod(position.Rot.y, XM_2PI);
			position.Rot.z = std::fmod(position.Rot.z, XM_2PI);
		}
		// update the entities region
		for (auto & regionChangeEntity : regionChanges) {
			EM->UpdatePosition(regionChangeEntity.first, regionChangeEntity.second);
		}

		/*},
			m_entities.GetComponentMask(),
			m_entities.GetComponentMask(),
			m_entities.GetComponentMask())
		);*/

		// check to see if the player has moved enough for an entity resync
		// alternate case passes if no regions have been loaded
			
		/*if (m_moveTracker.Update(playerPos) || !EM->RegionsLoaded()) {
			IEventManager::Invoke(EventTypes::Movement_PlayerMoved, playerPos.x , playerPos.z);
			
		}*/

	}

	void MovementSystem::SyncEntities()
	{
		EM->UpdateCache(m_entities);
	}

	void MovementSystem::Initialize()
	{
		SyncEntities();
	}

}

