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
		TaskManager::Get().Push(Task([=] {
			for (auto & entity : m_entities) {
				auto & position = entity.Get<Position>();
				auto & movement = entity.Get<Movement>();
				movement.Velocity += movement.Acceleration * elapsed;
				position.Pos += movement.Velocity * elapsed;

				position.Rot += movement.AngularVelocity * elapsed;
				float limit = XM_PI / 2.0f - 0.01f;
				position.Rot.y = std::max(-limit, position.Rot.y);
				position.Rot.y = std::min(limit, position.Rot.y);

				// keep longitude in sane range by wrapping
				if (position.Rot.x > XM_PI)
				{
					position.Rot.x -= XM_PI * 2.0f;
				}
				else if (position.Rot.x < -XM_PI)
				{
					position.Rot.x += XM_PI * 2.0f;
				}
			}
		},
			m_entities.GetComponentMask(),
			m_entities.GetComponentMask(),
			m_entities.GetComponentMask())
		);

		// check to see if the player has moved enough for an entity resync
		// alternate case passes if no regions have been loaded
		if (m_moveTracker.Update(EM->PlayerPos()) || !EM->RegionsLoaded()) {
			IEventManager::Invoke(EventTypes::Movement_PlayerMoved, m_moveTracker.lastX,m_moveTracker.lastZ);
			SyncEntities();
		}

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

