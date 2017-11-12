#pragma once
#include "EntityManager.h"
#include "Component.h"
#include "Entity.h"
class System
{
public:
	System(shared_ptr<EntityManager> & entityManager, vector<string> & components, unsigned short updatePeriod);
	virtual ~System() {}
	void Tick(double & elapsed);
	virtual void Initialize();
	virtual void Save();
	// Called before Update()
	virtual void SyncEntities();
	virtual string Name() = 0;
protected:
	virtual void Update(double & elapsed) = 0;
	double m_elapsed;				// seconds elapsed since last update
	shared_ptr<EntityManager> EM;
	unsigned long m_componentMask;
	vector<shared_ptr<Entity>> m_entities;
	//----------------------------------------------------------------
	// Helper functions

private:
	unsigned short m_updatePeriod;	// number of update ticks between Update();
	unsigned short m_ticksLeft;		// number of update ticks left until next Update();
};

