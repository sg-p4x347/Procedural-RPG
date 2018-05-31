#pragma once
#include "WorldSystem.h"
class SystemManager;
class CollisionSystem :
	public WorldSystem
{
public:
	CollisionSystem(SystemManager * systemManager, WorldEntityManager * entityManager, vector<string> & components, unsigned short updatePeriod);

	// Inherited via WorldSystem
	virtual string Name() override;
	virtual void Update(double & elapsed) override;
	virtual void SyncEntities() override;
private:
	SystemManager * SM;
	vector<EntityPtr> m_entities;
};

