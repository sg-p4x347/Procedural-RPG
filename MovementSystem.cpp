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
		//TaskManager::Get().Push(Task([=] {
		for (auto & entity : m_entities) {
			auto & position = entity.Get<Position>();
			auto & movement = entity.Get<Movement>();
			movement.Velocity += movement.Acceleration * elapsed;
			position.Pos += movement.Velocity * elapsed;

			position.Rot += movement.AngularVelocity * elapsed;
			position.Rot.x = std::fmod(position.Rot.x, XM_2PI);
			position.Rot.y = std::fmod(position.Rot.y, XM_2PI);
			position.Rot.z = std::fmod(position.Rot.z, XM_2PI);
		}
		/*},
			m_entities.GetComponentMask(),
			m_entities.GetComponentMask(),
			m_entities.GetComponentMask())
		);*/

		// check to see if the player has moved enough for an entity resync
		// alternate case passes if no regions have been loaded
			auto playerPos = EM->PlayerPos();
		if (m_moveTracker.Update(playerPos) || !EM->RegionsLoaded()) {
			IEventManager::Invoke(EventTypes::Movement_PlayerMoved, playerPos.x , playerPos.z);
			SyncEntities();
		}

	}

	void MovementSystem::SyncEntities()
	{
		EM->UpdateGlobalCache(m_entities);
	}

	void MovementSystem::Initialize()
	{
		SyncEntities();
	}

}

