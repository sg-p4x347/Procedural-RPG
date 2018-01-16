#pragma once
#include "WorldSystem.h"
#include "RenderSystem.h"
class MovementSystem :
	public WorldSystem
{
public:
	MovementSystem(shared_ptr<EntityManager> & entityManager, vector<string> & components, unsigned short updatePeriod,shared_ptr<RenderSystem> renderSys);

	// Inherited via System
	virtual string Name() override;
	virtual void Update(double & elapsed) override;
	virtual void SyncEntities() override;
private:
	vector<EntityPtr> m_entities;
	Vector3 m_lastPos;
	static const float m_updateRange;
	shared_ptr<RenderSystem> m_renderSystem;
};

