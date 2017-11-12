#pragma once
#include "System.h"
class MovementSystem :
	public System
{
public:
	MovementSystem(shared_ptr<EntityManager> & entityManager, vector<string> & components, unsigned short updatePeriod);

	// Inherited via System
	virtual string Name() override;
	virtual void Update(double & elapsed) override;
};

