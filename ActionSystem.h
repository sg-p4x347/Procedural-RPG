#pragma once
#include "WorldSystem.h"
/*----------------------------------------------------------------
 Purpose:
 Provide standard operations on world action nodes. Designed to be
 utilized by other systems for the creation and life-cycle of
 action node entities
 ----------------------------------------------------------------*/
class SystemManager;

class ActionSystem :
	public WorldSystem
{
public:
	ActionSystem(shared_ptr<SystemManager> systemManager, unique_ptr<WorldEntityManager> & entityManager, vector<string> & components, unsigned short updatePeriod);

	// Inherited via WorldSystem
	virtual string Name() override;
	virtual void Update(double & elapsed) override;

	void Check();
	void CreateAction();
private:
	shared_ptr<SystemManager> SM;
};

