#include "pch.h"
#include "CollisionSystem.h"
#include "TerrainSystem.h"
#include "SystemManager.h"
#include "Game.h"
#include "IEventManager.h"
#include "CollisionUtil.h"
namespace world {
	CollisionSystem::CollisionSystem(SystemManager * systemManager, WEM * entityManager, unsigned short updatePeriod) :
		WorldSystem::WorldSystem(entityManager, updatePeriod), 
		SM(systemManager), 
		m_dynamic(entityManager->NewEntityCache<Position,Collision,Movement>()),
		m_static(entityManager->NewEntityCache<Position, Collision>())
	{
		IEventManager::RegisterHandler(EventTypes::WEM_Resync, std::function<void(void)> ([=]() {
			SyncEntities();
		}));

		//// TEMP
		//std::vector<Vector3> hullA{ 
		//	Vector3(0,0,0),
		//	Vector3(0,0,2),
		//	Vector3(0,2,0),
		//	Vector3(0,2,2),
		//	Vector3(2,0,0),
		//	Vector3(2,0,2),
		//	Vector3(2,2,0),
		//	Vector3(2,2,2)
		//};
		//std::vector<Vector3> hullB{ 
		//	Vector3(1.75,1.5,1.5),
		//	Vector3(1.5,1.95,2),
		//	Vector3(1.58,2,1.5),
		//	Vector3(1.5,2.1,2),
		//	Vector3(2,1.75,1.5),
		//	Vector3(2,1.5,2),
		//	Vector3(2,2,1.5),
		//	Vector3(2,2,2)
		//};

		//CollisionUtil::GjkIntersection intersection;
		//if (CollisionUtil::GJK(hullA, hullB, intersection)) {
		//	Utility::OutputLine("WHOOP");
		//}
	}

	string CollisionSystem::Name()
	{
		return "Collision";
	}

	void CollisionSystem::Update(double & elapsed)
	{
		auto terrainSystem = SM->GetSystem<TerrainSystem>("Terrain");
		for (auto & entity : m_dynamic) {
			auto & collision = entity.Get<Collision>();
			collision.Colliding = 0;
			if (collision.Enabled) {
				auto & position = entity.Get<Position>();
				// Bound X and Z to the world area
				position.Pos.x = std::max(0.f, std::min((float)terrainSystem->Width(), position.Pos.x));
				position.Pos.z = std::max(0.f, std::min((float)terrainSystem->Width(), position.Pos.z));

				// Get the center bottom of the box for terrain collison
				Vector3 bottomCenter =
					position.Pos +
					collision.BoundingBox.Position;
				bottomCenter.y -= collision.BoundingBox.Size.y * 0.5;

				float terrainHeight = terrainSystem->Height(bottomCenter.x, bottomCenter.z);

				// Pop the y back to the terrain
				auto & movement = entity.Get<Movement>();
				if (terrainHeight > bottomCenter.y) {
					if (std::abs(movement.Velocity.y) > 5.f) {
						IEventManager::Invoke(EventTypes::Sound_PlayEffect, string("Hit0"));
					}
					position.Pos.y = terrainHeight + (collision.BoundingBox.Size.y * 0.5f - collision.BoundingBox.Position.y);

					movement.Velocity.y = 0.f;

				}
				
				
				//----------------------------------------------------------------
				// Dynamic vs Static
				Vector3 futurePos = position.Pos + movement.Velocity * elapsed;
				Matrix world = Matrix::CreateFromYawPitchRoll(position.Rot.y, position.Rot.x, position.Rot.z) * Matrix::CreateTranslation(futurePos);
				auto hullA = BoxVertices(collision.BoundingBox, world);
				float radius = collision.BoundingBox.Size.Length() * 0.5f;
				
				for (auto & other : m_static) {
					auto & otherPos = other.Get<Position>();
					auto & otherCollision = other.Get<Collision>();
					auto otherWorld = Matrix::CreateFromYawPitchRoll(otherPos.Rot.y, otherPos.Rot.x, otherPos.Rot.z) * Matrix::CreateTranslation(otherPos.Pos);
					otherCollision.Colliding = 0;
					float otherRadius = otherCollision.BoundingBox.Size.Length() * 0.5f;
					// Radial broad phase check
					if (((position.Pos + collision.BoundingBox.Position) - (otherPos.Pos + otherCollision.BoundingBox.Position)).Length() <= radius + otherRadius) {
						auto hullB = BoxVertices(otherCollision.BoundingBox, otherWorld);
						CollisionUtil::GjkIntersection intersection;
						if (CollisionUtil::GJK(hullA, hullB, intersection)) {
							collision.Colliding = 1;
							otherCollision.Colliding = 1;

							// determine the seperating axis from the current position
							world = Matrix::CreateFromYawPitchRoll(position.Rot.y, position.Rot.x, position.Rot.z) * Matrix::CreateTranslation(position.Pos);
							hullA = BoxVertices(collision.BoundingBox, world);							
							CollisionUtil::SatResult satResult;
							vector<Vector3> axes = CollisionUtil::GenerateSatAxes(world, otherWorld);
							
							if (!CollisionUtil::SatIntersection(hullA, hullB, axes, satResult)) {
								movement.Velocity = satResult.Axis;
							}
							break;
						}
						else {
							// culled by gjk
							collision.Colliding = std::min(2, collision.Colliding);
							otherCollision.Colliding = 2;
						}
					}
					else {
						// culled by broad phase
						collision.Colliding = std::min(3, collision.Colliding);
						otherCollision.Colliding = 3;
					}
				}

				if (entity.GetSignature() == EM->PlayerSignature()) {
					// Jump if close to the ground
					bottomCenter =
						position.Pos +
						collision.BoundingBox.Position;
					bottomCenter.y -= collision.BoundingBox.Size.y * 0.5;
					if (std::abs(terrainHeight - bottomCenter.y) <= 0.1f && Game::Get().KeyboardTracker.IsKeyPressed(DirectX::Keyboard::Keys::Space)) {
						movement.Velocity.y += 5.f;
					}
				}
			}
		}
	}

	void CollisionSystem::SyncEntities()
	{
		EM->UpdateCache(m_dynamic);
		MaskType staticMask = EM->GetMask<Position, Collision>();
		MaskType movementMask = EM->GetMask<Movement>();
		EM->UpdateCache(m_static, [=](world::MaskType & sig) {
			return ((sig & staticMask) == staticMask) && !(sig & movementMask);
		});
	}
	std::vector<Vector3> CollisionSystem::BoxVertices(Box & box, Matrix & transform)
	{
		std::vector<Vector3> vertices = std::vector<Vector3>(8);
		int vertexIndex = 0;
		for (float x = 0.f; x <= box.Size.x; x += box.Size.x) {
			for (float y = 0.f; y <= box.Size.y; y += box.Size.y) {
				for (float z = 0.f; z <= box.Size.z; z += box.Size.z) {
					Vector3 & vertex = vertices[vertexIndex];
					vertex.x = box.Position.x - box.Size.x * 0.5 + x;
					vertex.y = box.Position.y - box.Size.y * 0.5 + y;
					vertex.z = box.Position.z - box.Size.z * 0.5 + z;

					vertex = Vector3::Transform(vertex,transform);

					vertexIndex++;
				}
			}
		}
		return vertices;
	}
}