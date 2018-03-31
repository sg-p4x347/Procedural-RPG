#include "pch.h"
#include "MovementSystem.h"
#include "Position.h"
#include "Movement.h"
#include "IEventManager.h"
#include "TaskManager.h"
const float MovementSystem::m_updateRange = 5.f;

MovementSystem::MovementSystem(
	unique_ptr<WorldEntityManager> &  entityManager,
	vector<string>& components, 
	unsigned short updatePeriod,
	shared_ptr<RenderSystem> renderSys) : WorldSystem::WorldSystem(entityManager,components,updatePeriod), m_renderSystem(renderSys)
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
			auto position = entity->GetComponent<Components::Position>("Position");
			auto movement = entity->GetComponent<Components::Movement>("Movement");
			movement->Velocity += movement->Acceleration * elapsed;
			position->Pos += movement->Velocity * elapsed;

			position->Rot += movement->AngularVelocity * elapsed;
		}
	//}, m_componentMask, m_componentMask));
	
	// check to see if the player has moved enough for an entity resync
	if (m_lastPos == Vector3::Zero && EM->Player()) {
		m_lastPos = EM->PlayerPos()->Pos;
	}
	if (Vector3::Distance(EM->PlayerPos()->Pos, m_lastPos) > m_updateRange) {
		m_renderSystem->SyncEntities();
		m_lastPos = EM->PlayerPos()->Pos;
	}
}

void MovementSystem::SyncEntities()
{
	m_entities = EM->FindEntities(m_componentMask);
}

void MovementSystem::Initialize()
{
	SyncEntities();
}

