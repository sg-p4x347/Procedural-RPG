#include "pch.h"
#include "CollisionSystem.h"
#include "TerrainSystem.h"
#include "SystemManager.h"
#include "Game.h"
#include "IEventManager.h"
CollisionSystem::CollisionSystem(SystemManager * systemManager, unique_ptr<WorldEntityManager>& entityManager, vector<string>& components, unsigned short updatePeriod) :
	WorldSystem::WorldSystem(entityManager, components, updatePeriod), SM(systemManager)
{
}

string CollisionSystem::Name()
{
	return "Collision";
}

void CollisionSystem::Update(double & elapsed)
{
	auto terrainSystem = SM->GetSystem<TerrainSystem>("Terrain");
	for (auto & entity : m_entities) {
		auto collision = entity->GetComponent<Components::Collision>("Collision");
		if (collision->Enabled) {
			auto position = entity->GetComponent<Components::Position>("Position");


			// Get the center bottom of the box for terrain collison
			Vector3 bottomCenter =
				position->Pos +
				collision->BoundingBox.Position +
				0.5 * Vector3(collision->BoundingBox.Size.x, 0.f, collision->BoundingBox.Size.z);

			float terrainHeight = terrainSystem->Height(bottomCenter.x, bottomCenter.z);

			// Pop the y back to the terrain
			auto movement = entity->GetComponent<Components::Movement>("Movement");
			if (terrainHeight > bottomCenter.y) {
				if (std::abs(movement->Velocity.y) > 5.f) {
					IEventManager::Invoke(EventTypes::Sound_PlayEffect, string("Hit0"));
				}
				position->Pos.y = terrainHeight - collision->BoundingBox.Position.y;
				
				movement->Velocity.y = 0.f;
				
			}
			// Jump if close to the ground
			bottomCenter =
				position->Pos +
				collision->BoundingBox.Position +
				0.5 * Vector3(collision->BoundingBox.Size.x, 0.f, collision->BoundingBox.Size.z);
			if (std::abs(terrainHeight - bottomCenter.y) <= 0.1f && Game::Get().KeyboardTracker.IsKeyPressed(DirectX::Keyboard::Keys::Space)) {
				movement->Velocity.y += 5.f;
			}
			// Bound X and Z to the world area
			position->Pos.x = std::max(0.f, std::min((float)terrainSystem->Width(), position->Pos.x));
			position->Pos.z = std::max(0.f, std::min((float)terrainSystem->Width(), position->Pos.z));
		}
	}
}

void CollisionSystem::SyncEntities()
{
	m_entities = EM->FindEntities(m_componentMask);
}
