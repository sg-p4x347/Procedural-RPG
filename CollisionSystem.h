#pragma once
#include "WorldSystem.h"
#include "WorldDomain.h"
#include "CollisionModel.h"

class SystemManager;
namespace world {
	struct ContactInfo {
		ContactInfo() : colliding(false), contactNormal(Vector3::Zero) {}
		bool colliding;
		Vector3 contactNormal;
	};
	class CollisionSystem :
		public WorldSystem
	{
	public:
		CollisionSystem(SystemManager * systemManager, WEM * entityManager, unsigned short updatePeriod);

		// Inherited via WorldSystem
		virtual string Name() override;
		virtual void Update(double & elapsed) override;
		virtual void SyncEntities() override;
	private:
		SystemManager * SM;
		WorldEntityCache<WEM::RegionType, Position, Collision,Movement> m_dynamic;
		WorldEntityCache<WEM::RegionType, Position, Collision> m_static;
		WorldEntityCache<WEM::RegionType, Position, VoxelGridModel> m_grids;
		std::map<AssetID, shared_ptr<geometry::CollisionModel>> m_collisionAssets;
	private:
		std::vector<Vector3> BoxVertices(BoundingBox & box,Matrix & transform);
		shared_ptr<geometry::CollisionModel> GetCollisionAsset(AssetID asset);
		// Bakes individual collision assets together within Grid space and caches the result on the voxel
		shared_ptr<geometry::CollisionModel> GetCollisionAsset(ModelVoxel & voxel);
		ContactInfo CheckCollision(
			shared_ptr<geometry::CollisionVolume> volumeA,
			shared_ptr<geometry::CollisionVolume> volumeAprime,
			shared_ptr<geometry::CollisionVolume> volumeB
		);
		void HandleCollision(
			Position & position,
			Collision & dynamicCollision,
			Movement & dynamicMovement,
			shared_ptr<geometry::CollisionVolume> & dynamicCollisionVolume,
			shared_ptr<geometry::CollisionVolume> & dynamicCollisionVolumePrime,
			Position & staticPosition,
			shared_ptr<geometry::CollisionModel> & staticCollisionModel
		);
	};
}
