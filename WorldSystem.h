#pragma once
#include "System.h"
class WorldSystem :
	public System
{
public:
	WorldSystem(unique_ptr<EntityManager> & entityManager, vector<string> & components, unsigned short updatePeriod);
protected:
	unique_ptr<EntityManager> & EM;
};

