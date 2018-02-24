#pragma once
#include "WorldSystem.h"
#include "WorldEntityManager.h"
class BuildingSystem :
	public WorldSystem
{
public:
	BuildingSystem(unique_ptr<WorldEntityManager> &  entityManager,vector<string> components, unsigned short updatePeriod);

	// Inherited via WorldSystem
	virtual string Name() override;
	virtual void Update(double & elapsed) override;
};

