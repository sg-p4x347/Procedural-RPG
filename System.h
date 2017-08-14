#pragma once
#include "EntityManager.h"
#include "Component.h"

class System
{
public:
	System(shared_ptr<EntityManager> & entityManager, vector<string> & components, unsigned short updatePeriod);
	void Tick(double & elapsed);
	
	~System();
protected:
	virtual void Update() = 0;
	double m_elapsed;				// seconds elapsed since last update
	shared_ptr<EntityManager> m_entityManager;
	vector<string> m_components;
	vector<unsigned int> m_entities;
private:
	unsigned short m_updatePeriod;	// number of update ticks between Update();
	unsigned short m_ticksLeft;		// number of update ticks left until next Update();
};

