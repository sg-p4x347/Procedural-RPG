#pragma once
#include "WorldEntityManager.h"
#include "Component.h"
#include "Entity.h"
class System
{
public:
	System(unsigned short updatePeriod);
	virtual ~System() {}
	void Tick(double & elapsed);
	virtual void Initialize();
	virtual void Save();
	// Called before Update()
	virtual void SyncEntities();
	virtual string Name() = 0;

	// Update control
	void Halt();
	void Run();
protected:
	virtual void Update(double & elapsed) = 0;
	double m_elapsed;				// seconds elapsed since last update
	unsigned long m_componentMask;
	//----------------------------------------------------------------
	// Helper functions

private:
	unsigned short m_updatePeriod;	// number of update ticks between Update();
	unsigned short m_ticksLeft;		// number of update ticks left until next Update();
	bool m_halted;
};

