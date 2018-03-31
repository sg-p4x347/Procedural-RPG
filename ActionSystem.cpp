#include "pch.h"
#include "ActionSystem.h"
#include "SystemManager.h"
#include "IEventManager.h"
#include "Game.h"
#include "Box.h"
ActionSystem::ActionSystem(
	SystemManager * systemManager,
	unique_ptr<WorldEntityManager>& entityManager, 
	vector<string>& components,
	unsigned short updatePeriod) : 
	WorldSystem::WorldSystem(entityManager,components,updatePeriod),
	SM(systemManager)
{
	IEventManager::RegisterHandler(EventTypes::Action_Check, std::function<void(void)>([=] {
		Check();
	}));

	
}

string ActionSystem::Name()
{
	return "Action";
}

void ActionSystem::Update(double & elapsed)
{
	EntityPtr actionNode;
	if (CanInteract(actionNode)) {
		IEventManager::Invoke(GUI_ShowHint, string("E"));
	}
	else {
		IEventManager::Invoke(GUI_HideHint);
	}
}

void ActionSystem::Check()
{
	//for (auto & actionNode : EM->FindEntitiesInRange(EM->ComponentMask("Action"), EM->PlayerPos()->Pos, 5.f)) {
	EntityPtr actionNode;
	if (CanInteract(actionNode)) {
		shared_ptr<Components::Action> action = actionNode->GetComponent<Components::Action>("Action");
		EntityPtr target;
		if (!EM->Find(action->TargetEntity, target)) {
			target = actionNode;
		}
		// (EntityPtr actor, EntityPtr target)
		IEventManager::Invoke(action->Event, EM->Player(), actionNode);
	}
	//}
}

void ActionSystem::CreateAction(Vector3 position, Vector3 size, EventTypes event, EntityPtr targetEntity)
{
	auto action = EM->NewEntity();
	action->AddComponent(new Components::Position(position));
	action->AddComponent(new Components::Action(size, event, targetEntity->ID()));
}

Vector3 ActionSystem::GetPlayerLookRay()
{
	auto position = EM->PlayerPos();
	auto quat = SimpleMath::Quaternion::CreateFromYawPitchRoll(position->Rot.x, -position->Rot.y, 0.f);
	return Vector3::Transform(Vector3::UnitZ, quat);
}

bool ActionSystem::CanInteract(EntityPtr & actionNode)
{
	const float range = 1.f;
	Vector3 lookVec = GetPlayerLookRay();
	auto closeActions = EM->FindEntitiesInRange(EM->ComponentMask(vector<string>{"Action"}), EM->PlayerPos()->Pos, 5.f);
	for (auto & entity : closeActions) {
		Vector3 position = entity->GetComponent<Components::Position>("Position")->Pos;
		auto action = entity->GetComponent<Components::Action>("Action");
		Vector3 size = action->Size;
		Box box = Box(position - size * 0.5f, action->Size); // Center the box on the position
		if (box.Intersects(Line(EM->PlayerPos()->Pos, EM->PlayerPos()->Pos + lookVec * range))) {
			actionNode = entity;
			return true;
		}
	}
	return false;
}
