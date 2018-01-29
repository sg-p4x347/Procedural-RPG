#pragma once
#include "System.h"
class WorldSystem :
	public System
{
public:
	WorldSystem(unique_ptr<WorldEntityManager> & entityManager, vector<string> & components, unsigned short updatePeriod);
protected:
	unique_ptr<WorldEntityManager> & EM;
};

