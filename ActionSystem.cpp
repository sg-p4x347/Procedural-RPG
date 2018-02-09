#include "pch.h"
#include "ActionSystem.h"
#include "SystemManager.h"
#include "IEventManager.h"
#include "Movement.h"
#include "Game.h"
ActionSystem::ActionSystem(
	shared_ptr<SystemManager> systemManager,
	unique_ptr<WorldEntityManager>& entityManager, 
	vector<string>& components,
	unsigned short updatePeriod) : 
	WorldSystem::WorldSystem(entityManager,components,updatePeriod),
	SM(systemManager)
{
	IEventManager::RegisterHandler(EventTypes::Action_Check, std::function<void(void)>([=] {
		auto player = EM->Player();
		player->GetComponent<Components::Movement>("Movement")->Acceleration.y = -9.8;
	}));
}

string ActionSystem::Name()
{
	return "Action";
}

void ActionSystem::Update(double & elapsed)
{

	EM->FindEntitiesInRange(EM->ComponentMask(vector<string>{"Action"}), EM->PlayerPos()->Pos, 10.f);
}

void ActionSystem::Check()
{
}
