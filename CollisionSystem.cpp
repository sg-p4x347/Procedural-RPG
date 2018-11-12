#include "pch.h"
#include "CollisionSystem.h"
#include "TerrainSystem.h"
#include "SystemManager.h"
#include "Game.h"
#include "IEventManager.h"
#include "TaskManager.h"
#include "CollisionUtil.h"
#include "AssetManager.h"
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
		MaskType writeMask = EM->GetMask<Movement, Collision>();
		TaskManager::Get().RunSynchronous(Task([=] {
			auto terrainSystem = SM->GetSystem<TerrainSystem>("Terrain");
			for (auto & entity : m_dynamic) {
				auto & collision = entity.Get<Collision>();
				collision.Colliding = 0;

				if (collision.Enabled) {
					auto & position = entity.Get<Position>();
					// Bound X and Z to the world area
					position.Pos.x = std::max(0.f, std::min((float)terrainSystem->Width(), position.Pos.x));
					position.Pos.z = std::max(0.f, std::min((float)terrainSystem->Width(), position.Pos.z));

					BoundingBox boundingBox = GetCollisionAsset(collision).AxisAlignedBoxes[0];
					// Get the center bottom of the box for terrain collison
					Vector3 bottomCenter =
						position.Pos +
						boundingBox.Center;
					bottomCenter.y -= boundingBox.Extents.y * 0.5;

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
							position.Pos.y = -0.01f + terrainHeight + (boundingBox.Extents.y * 0.5f - boundingBox.Center.y);

							movement.Velocity.y = 0.f;
							collision.CollisionNormals.push_back(Vector3::Up);
						}


						//----------------------------------------------------------------
						// Dynamic vs Static


						Vector3 futurePos = position.Pos + movement.Velocity * elapsed;
						Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(position.Rot.y, position.Rot.x, position.Rot.z);
						Matrix boxTranslation = Matrix::CreateTranslation(boundingBox.Center);
						Matrix world = boxTranslation * (rotationMatrix * Matrix::CreateTranslation(futurePos));
						auto hullA = BoxVertices(boundingBox, world);
						float radius = Vector3(boundingBox.Extents).Length() * 0.5f;

						for (auto & other : m_static) {
							auto & otherPos = other.Get<Position>();
							auto & otherCollision = other.Get<Collision>();
							BoundingBox staticBoundingBox = GetCollisionAsset(otherCollision).AxisAlignedBoxes[0];
							auto otherRotationMatrix = Matrix::CreateFromYawPitchRoll(otherPos.Rot.y, otherPos.Rot.x, otherPos.Rot.z);
							auto otherWorld = Matrix::CreateTranslation(staticBoundingBox.Center) * (otherRotationMatrix * Matrix::CreateTranslation(otherPos.Pos));
							otherCollision.Colliding = 0;
							float otherRadius = Vector3(staticBoundingBox.Extents).Length() * 0.5f;
							// Radial broad phase check
							if (((position.Pos + boundingBox.Center) - (otherPos.Pos + staticBoundingBox.Center)).Length() <= radius + otherRadius) {
								auto hullB = BoxVertices(staticBoundingBox, otherWorld);
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

									hullA = BoxVertices(boundingBox, world);
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
		},
			m_dynamic.GetComponentMask(),
			m_dynamic.GetComponentMask(),
			writeMask)
		);
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
	std::vector<Vector3> CollisionSystem::BoxVertices(BoundingBox & box, Matrix & transform)
	{
		std::vector<Vector3> vertices = std::vector<Vector3>(8);
		int vertexIndex = 0;
		for (float x = 0.f; x <= box.Extents.x; x += box.Extents.x) {
			for (float y = 0.f; y <= box.Extents.y; y += box.Extents.y) {
				for (float z = 0.f; z <= box.Extents.z; z += box.Extents.z) {
					Vector3 & vertex = vertices[vertexIndex];
					vertex.x = -box.Extents.x * 0.5 + x;
					vertex.y = -box.Extents.y * 0.5 + y;
					vertex.z = -box.Extents.z * 0.5 + z;

					vertex = Vector3::Transform(vertex,transform);

					vertexIndex++;
				}
			}
		}
		return vertices;
	}
	CollisionAsset CollisionSystem::GetCollisionAsset(Collision & collision)
	{
		auto it = m_collisionAssets.find(collision.Asset);
		if (it != m_collisionAssets.end())
			return it->second;

		EntityPtr asset;
		if (AssetManager::Get()->Find(collision.Asset, collision.Type, asset)) {
			CollisionAsset collisionAsset = *(asset->GetComponent<CollisionAsset>("CollisionAsset"));
			m_collisionAssets.insert(std::map<EntityID,CollisionAsset>::value_type(collision.Asset, collisionAsset));
			return collisionAsset;
		}
		// default if asset was not found
		return CollisionAsset();
	}
}