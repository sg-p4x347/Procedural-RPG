#include "pch.h"
#include "CollisionSystem.h"
#include "TerrainSystem.h"
#include "MovementSystem.h"

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
		m_static(entityManager->NewEntityCache<Position, Collision>()),
		m_grids(entityManager->NewEntityCache<Position, VoxelGridModel>())
	
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
				auto & movement = entity.Get<Movement>();
				collision.Colliding = 0;
				if (collision.Enabled) {
					auto & position = entity.Get<Position>();
					// Bound X and Z to the world area
					position.Pos.x = std::max(0.f, std::min((float)terrainSystem->Width(), position.Pos.x));
					position.Pos.z = std::max(0.f, std::min((float)terrainSystem->Width(), position.Pos.z));
					vector<ContactInfo> contacts;
					for (auto & volume : GetCollisionAsset(collision.Asset)->volumes) {
						// Get the center bottom of the box for terrain collison
						Vector3 bottomCenter = position.Pos;
						float yRadius = volume->Support(Vector3::Down).y;
						bottomCenter.y += yRadius;

						float terrainHeight = terrainSystem->Height(bottomCenter.x, bottomCenter.z);

						
						if (movement.Velocity.LengthSquared() > 0.f) {
							
							// Pop the y back to the terrain
							if (terrainHeight > bottomCenter.y) {
								if (std::abs(movement.Velocity.y) > 5.f) {
									IEventManager::Invoke(EventTypes::Sound_PlayEffect, string("Hit0"));
								}
								// small fudge factor to keep in contact with the ground
								position.Pos.y = terrainHeight - yRadius;

								movement.Velocity.y = std::max(0.f,movement.Velocity.y);
								ContactInfo contact;
								contact.contactNormal = Vector3::Up;
								contacts.push_back(contact);
							}

							//----------------------------------------------------------------
							// Dynamic vs Static

							
							auto currentMatrix = MovementSystem::GetWorldMatrix(position.Pos,position.Rot);
							auto futurePos = MovementSystem::UpdatedPosition(position.Pos, movement.Velocity, elapsed);
							auto primeMatrix = MovementSystem::GetWorldMatrix(futurePos, position.Rot);
							auto dynamicVolume = volume->Transform(currentMatrix);
							auto dynamicVolumePrime = volume->Transform(primeMatrix);
							// Static colliders
							for (auto & other : m_static) { 
								auto & staticPos = other.Get<Position>();
								auto & staticCollision = other.Get<Collision>();
								auto staticCollisionModel = GetCollisionAsset(staticCollision.Asset);
								if (staticCollisionModel) {
									CheckCollision(
										position,
										dynamicVolume,
										dynamicVolumePrime,
										staticPos,
										staticCollisionModel,
										contacts);
								}
							}
							// Static grids
							for (auto & grid : m_grids) {
								auto & staticPos = grid.Get<Position>();
								auto & gridModel = grid.Get<VoxelGridModel>();
								auto dynamicBounds = dynamicVolume->Bounds();
								dynamicBounds.Transform(dynamicBounds, MovementSystem::GetWorldMatrix(staticPos.Pos, staticPos.Rot).Invert());
								for (ModelVoxel & voxel : gridModel.Voxels.GetIntersection(dynamicBounds)) {
									auto staticCollisionModel = GetCollisionAsset(voxel);
									CheckCollision(
										position,
										dynamicVolume,
										dynamicVolumePrime,
										staticPos,
										staticCollisionModel,
										contacts);
								}
							}
						}
						
					}
					CollisionResponse(contacts, collision, movement);
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
		MaskType gridMask = EM->GetMask<Position, VoxelGridModel>();
		MaskType movementMask = EM->GetMask<Movement>();
		EM->UpdateCache(m_static, [=](world::MaskType & sig) {
			return ((sig & staticMask) == staticMask) && !(sig & movementMask);
		});
		EM->UpdateCache(m_grids, gridMask);
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
	shared_ptr<geometry::CollisionModel> CollisionSystem::GetCollisionAsset(AssetID asset)
	{
		auto it = m_collisionAssets.find(asset);
		if (it != m_collisionAssets.end())
			return it->second;

		auto model = AssetManager::Get()->GetCMF(asset, AssetType::Authored);
		if (model) {
			m_collisionAssets.insert(std::make_pair(asset, model->GetCollision()));
			return model->GetCollision();
		}
		// default if asset was not found
		return nullptr;
	}
	shared_ptr<geometry::CollisionModel> CollisionSystem::GetCollisionAsset( ModelVoxel & voxel)
	{
		if (voxel.GetCollision() == nullptr) {
			geometry::CollisionModel collision;
		
			for (auto & component : voxel.GetComponents()) {

				auto compCollision = GetCollisionAsset(component.first);
				if (compCollision) {
					// transform the component collision asset within the voxel space
					collision.Merge(compCollision->Transform(TRANSFORMS[component.second]));
				}
			}
			// transform the voxel collision model into grid space
			voxel.SetCollision(shared_ptr<geometry::CollisionModel>(new geometry::CollisionModel(collision.Transform(Matrix::CreateTranslation(voxel.GetPosition() + Vector3(0.5f,0.5f,0.5f))))));
		}
		return voxel.GetCollision();
	}
	bool CollisionSystem::CheckCollision(
		ContactInfo & contact,
		shared_ptr<geometry::CollisionVolume> volumeA,
		shared_ptr<geometry::CollisionVolume> volumeAprime, 
		shared_ptr<geometry::CollisionVolume> volumeB
	)
	{
		// AABB Broad Phase
		if (volumeAprime->Bounds().Intersects(volumeB->Bounds())) {
			geometry::GjkIntersection intersection;
			geometry::SatResult satResult;

			/*if (CollisionUtil::SatIntersection(hullA, hullB, axes, satResult)) {
				collision.Colliding = 1;
				otherCollision.Colliding = 1;
			}*/
			if (geometry::SatIntersection(volumeAprime,volumeB, satResult)) {
			//if (geometry::GJK(volumeAprime, volumeB, intersection)) {

				// determine the seperating axis from the current position
				geometry::SatResult satResult;

				if (!geometry::SatIntersection(volumeA, volumeB, satResult)) {
					contact.contactNormal = satResult.Axis;
				}
				else {
					// Could not detect a separating axis
					contact.contactNormal = Vector3::Zero;
				}
				return true;
			}
			else {
				// culled by narrow phase
				return false;
			}
		}
		else {
			// culled by broad phase
			return false;
		}
	}
	void CollisionSystem::CollisionResponse(
		vector<ContactInfo>& contacts,
		Collision & collision,
		Movement & movement)
	{
		collision.Contacts = contacts;
		for (auto & contact : contacts) {
			movement.Velocity -= contact.contactNormal * contact.contactNormal.Dot(movement.Velocity);
		}
			
	}
	void CollisionSystem::CheckCollision(
		Position & position,
		shared_ptr<geometry::CollisionVolume> & dynamicCollisionVolume,
		shared_ptr<geometry::CollisionVolume> & dynamicCollisionVolumePrime,
		Position & staticPosition,
		shared_ptr<geometry::CollisionModel> & staticCollisionModel,
		vector<ContactInfo> & contacts)
	{
		for (auto & volume : staticCollisionModel->volumes) {
			auto staticWorld = MovementSystem::GetWorldMatrix(staticPosition.Pos, staticPosition.Rot);
			auto staticVolume = volume->Transform(staticWorld);

			ContactInfo contact;
			if (CheckCollision(contact, dynamicCollisionVolume, dynamicCollisionVolumePrime, staticVolume)) {
				contacts.push_back(contact);
			}
		}
	}
}