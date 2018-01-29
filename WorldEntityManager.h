#pragma once
#include "PersistenceEntityManager.h"
class WorldEntityManager :
	public PersistenceEntityManager
{
public:
	WorldEntityManager(Filesystem::path directory);
};

