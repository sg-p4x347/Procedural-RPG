#include "pch.h"
#include "MovementSystem.h"
#include "Position.h"
#include "Movement.h"
const float MovementSystem::m_updateRange = 100.f;

MovementSystem::MovementSystem(
	shared_ptr<EntityManager>& entityManager, 
	vector<string>& components, 
	unsigned short updatePeriod,
	shared_ptr<RenderSystem> renderSys) : System::System(entityManager,components,updatePeriod), m_renderSystem(renderSys)
{
	
}

string MovementSystem::Name()
{
	return "Movement";
}

void MovementSystem::Update(double & elapsed)
{
	for (auto & entity : m_entities) {
		auto position = EM->GetComponent<Components::Position>(entity, "Position");
		auto movement = EM->GetComponent<Components::Movement>(entity, "Movement");
		movement->Velocity += movement->Acceleration * elapsed;
		position->Pos += movement->Velocity * elapsed;

		position->Rot += movement->AngularVelocity * elapsed;
	}
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

