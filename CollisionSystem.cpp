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

				
				auto & movement = entity.Get<Movement>();
				if (movement.Velocity.LengthSquared() > 0.f) {
					collision.CollisionNormals.clear();
					// Pop the y back to the terrain
					if (terrainHeight > bottomCenter.y) {
						if (std::abs(movement.Velocity.y) > 5.f) {
							IEventManager::Invoke(EventTypes::Sound_PlayEffect, string("Hit0"));
						}
						// small fudge factor to keep in contact with the ground
						position.Pos.y = -0.01f + terrainHeight + (collision.BoundingBox.Size.y * 0.5f - collision.BoundingBox.Position.y);

						movement.Velocity.y = 0.f;
						collision.CollisionNormals.push_back(Vector3::Up);
					}
				
				
					//----------------------------------------------------------------
					// Dynamic vs Static
				
					
					Vector3 futurePos = position.Pos + movement.Velocity * elapsed;
					Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(position.Rot.y, position.Rot.x, position.Rot.z);
					Matrix boxTranslation = Matrix::CreateTranslation(collision.BoundingBox.Position);
					Matrix world = boxTranslation * (rotationMatrix * Matrix::CreateTranslation(futurePos));
					auto hullA = BoxVertices(collision.BoundingBox, world);
					float radius = collision.BoundingBox.Size.Length() * 0.5f;

					for (auto & other : m_static) {
						auto & otherPos = other.Get<Position>();
						auto & otherCollision = other.Get<Collision>();
						auto otherRotationMatrix = Matrix::CreateFromYawPitchRoll(otherPos.Rot.y, otherPos.Rot.x, otherPos.Rot.z);
						auto otherWorld = Matrix::CreateTranslation(otherCollision.BoundingBox.Position) * (otherRotationMatrix * Matrix::CreateTranslation(otherPos.Pos));
						otherCollision.Colliding = 0;
						float otherRadius = otherCollision.BoundingBox.Size.Length() * 0.5f;
						// Radial broad phase check
						if (((position.Pos + collision.BoundingBox.Position) - (otherPos.Pos + otherCollision.BoundingBox.Position)).Length() <= radius + otherRadius) {
							auto hullB = BoxVertices(otherCollision.BoundingBox, otherWorld);
							CollisionUtil::GjkIntersection intersection;
							CollisionUtil::SatResult satResult;

							/*if (CollisionUtil::SatIntersection(hullA, hullB, axes, satResult)) {
								collision.Colliding = 1;
								otherCollision.Colliding = 1;
							}*/
							vector<Vector3> axes = CollisionUtil::GenerateSatAxes(rotationMatrix, otherRotationMatrix);
							if (CollisionUtil::SatIntersection(hullA, hullB, axes, satResult)) {
								//if (CollisionUtil::GJK(hullA, hullB, intersection)) {
								collision.Colliding = 1;
								otherCollision.Colliding = 1;

								// determine the seperating axis from the current position
								rotationMatrix = Matrix::CreateFromYawPitchRoll(position.Rot.y, position.Rot.x, position.Rot.z);
								world = boxTranslation * (rotationMatrix * Matrix::CreateTranslation(position.Pos));

								hullA = BoxVertices(collision.BoundingBox, world);
								CollisionUtil::SatResult satResult;


								if (!CollisionUtil::SatIntersection(hullA, hullB, axes, satResult)) {
									collision.CollisionNormals.push_back(satResult.Axis);
									movement.Velocity -= satResult.Axis * satResult.Axis.Dot(movement.Velocity);
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
				}
				if (entity.GetSignature() == EM->PlayerSignature()) {
					// Jump if there is a collision normal facing up-ish to push off of
					
					if (Game::Get().KeyboardTracker.IsKeyPressed(DirectX::Keyboard::Keys::Space)) {
						for (auto & normal : collision.CollisionNormals) {
							if (std::abs(normal.Dot(Vector3::Up)) >= 0.5f) {
								movement.Velocity.y += 5.f;
								break;
							}
						}
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
					vertex.x = -box.Size.x * 0.5 + x;
					vertex.y = -box.Size.y * 0.5 + y;
					vertex.z = -box.Size.z * 0.5 + z;

					vertex = Vector3::Transform(vertex,transform);

					vertexIndex++;
				}
			}
		}
		return vertices;
	}
}