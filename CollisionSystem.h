#pragma once
#include "WorldSystem.h"
#include "WorldDomain.h"
#include "Box.h"
class SystemManager;
namespace world {
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
	private:
		std::vector<Vector3> BoxVertices(Box & box,Matrix & transform);
	};
}
