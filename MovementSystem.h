#pragma once
#include "WorldSystem.h"
#include "RenderSystem.h"
struct MovementTracker {
	MovementTracker(int grid) {
		gridSize = grid;
		lastX = -1;
		lastZ = -1;
	}
	int gridSize;
	int lastX;
	int lastZ;
	bool Update(Vector3 position) {
		int newX = position.x / gridSize;
		int newZ = position.z / gridSize;
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
	MovementSystem(WorldEntityManager *  entityManager, vector<string> & components, unsigned short updatePeriod,shared_ptr<RenderSystem> renderSys);

	// Inherited via System
	virtual string Name() override;
	virtual void Update(double & elapsed) override;
	virtual void SyncEntities() override;
	virtual void Initialize() override;
private:
	vector<EntityPtr> m_entities;
	MovementTracker m_moveTracker;
	shared_ptr<RenderSystem> m_renderSystem;
};


