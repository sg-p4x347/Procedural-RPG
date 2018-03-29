#pragma once
#include "WorldSystem.h"
class ResourceSystem :
	public WorldSystem
{
public:
	ResourceSystem(unique_ptr<WorldEntityManager> & entityManager, vector<string> & components, unsigned short updatePeriod);

	// Inherited via WorldSystem
	virtual string Name() override;
	virtual void Update(double & elapsed) override;
	//----------------------------------------------------------------
	// Public Interface
	void Generate();
private:
	void RegisterHandlers();

};

