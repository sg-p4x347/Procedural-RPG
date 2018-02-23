#include "pch.h"
#include "BuildingSystem.h"

BuildingSystem::BuildingSystem(unique_ptr<WorldEntityManager> &  entityManager, vector<string> components, unsigned short updatePeriod) : WorldSystem::WorldSystem(entityManager,components,updatePeriod)
{

}

string BuildingSystem::Name()
{
	return "Building";
}

void BuildingSystem::Update(double & elapsed)
{
}
