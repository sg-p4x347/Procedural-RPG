#pragma once
#include "System.h"
class WorldSystem :
	public System
{
public:
	WorldSystem(shared_ptr<EntityManager> entityManager, vector<string> & components, unsigned short updatePeriod);
protected:
	shared_ptr<EntityManager> EM;
};

