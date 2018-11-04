#pragma once
#include "WorldSystem.h"
#include "RenderSystem.h"

namespace world {
	struct MovementTracker {
		MovementTracker(const unsigned int grid) : gridSize(grid) {
			lastX = -1;
			lastZ = -1;
		}
		const unsigned int gridSize;
		int lastX;
		int lastZ;
		bool Update(Vector3 & position) {
			int newX = std::floor(position.x / (float)gridSize);
			int newZ = std::floor(position.z / (float)gridSize);
			if (newX != lastX || newZ != lastZ) {
				lastX = newX;
				lastZ = newZ;
				return true;
			}
			else {
				return false;
			}
		}
	};
	class MovementSystem :
		public WorldSystem
	{
	public:
		MovementSystem(
			WEM *  entityManager, 
			unsigned short updatePeriod);

		// Inherited via System
		virtual string Name() override;
		virtual void Update(double & elapsed) override;
		virtual void SyncEntities() override;
		virtual void Initialize() override;
	private:
		WorldEntityCache<WEM::RegionType,Position,Movement> m_entities;
		MovementTracker m_moveTracker;
	};

}
