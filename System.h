#pragma once
#include "EntityManager.h"
#include "Component.h"
#include "Entity.h"
class System
{
public:
	System(shared_ptr<EntityManager> & entityManager, vector<string> & components, unsigned short updatePeriod);
	void Tick(double & elapsed);
	virtual void Initialize();
	// Called before Update()
	virtual void SyncEntities() = 0;
	virtual string Name() = 0;
	~System();
protected:
	virtual void Update() = 0;
	double m_elapsed;				// seconds elapsed since last update
	shared_ptr<EntityManager> m_entityManager;
	unsigned long m_componentMask;
	vector<shared_ptr<Entity>> m_entities;
	//----------------------------------------------------------------
	// Helper functions
private:
	unsigned short m_updatePeriod;	// number of update ticks between Update();
	unsigned short m_ticksLeft;		// number of update ticks left until next Update();
};

