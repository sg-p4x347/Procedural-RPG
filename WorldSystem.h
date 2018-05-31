#pragma once
#include "System.h"
class WorldSystem :
	public System
{
public:
	WorldSystem(WorldEntityManager * entityManager, vector<string> & components, unsigned short updatePeriod);
protected:
	WorldEntityManager * EM;
};

