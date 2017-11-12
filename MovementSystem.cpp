#include "pch.h"
#include "MovementSystem.h"
#include "Position.h"
#include "Movement.h"

MovementSystem::MovementSystem(
	shared_ptr<EntityManager>& entityManager, 
	vector<string>& components, 
	unsigned short updatePeriod) : System::System(entityManager,components,updatePeriod)
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
}

