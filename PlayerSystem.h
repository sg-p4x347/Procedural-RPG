#pragma once
#include "WorldSystem.h"
class SystemManager;
struct InputAction {
	InputAction() : InputAction::InputAction("", "", [] {}) {}
	InputAction(string name, string description, function<void()> action, bool edgeTrigger = false,bool worldOnly = true) {
		Name = name;
		Description = description;
		Action = action;
		EdgeTrigger = edgeTrigger;
		Registered = false;
		WorldOnly = worldOnly;
	}
	string Name;
	string Description;
	function<void()> Action;
	bool EdgeTrigger;
	bool Registered;

	bool WorldOnly;
};
class PlayerSystem :
	public WorldSystem
{
public:
	PlayerSystem(SystemManager * systemManager, unique_ptr<WorldEntityManager> &  entityManager,
		vector<string> & components, 
		unsigned short updatePeriod
	);

	// Inherited via System
	virtual string Name() override;
	virtual void Update(double & elapsed) override;
	//----------------------------------------------------------------
	// Generation
	void CreatePlayer();

	//----------------------------------------------------------------
	// Utility
	Quaternion GetPlayerQuaternion(bool ignorePitch = false);
	void Run() override;
	//----------------------------------------------------------------
	// Interaction state
	void SetInteractionState(Components::InteractionStates state);
private:
	SystemManager * SM;
	shared_ptr<DirectX::Mouse> m_mouse;
	shared_ptr<DirectX::Keyboard> m_keyboard;
	void RegisterHandlers();
	//----------------------------------------------------------------
	// Helpers
	shared_ptr<Components::Player> GetPlayerComp();
	//----------------------------------------------------------------
	// Movement
	Vector3 m_direction;
	void SetMovementToNormal();
	void SetMovementToSpectator();
	
	//----------------------------------------------------------------
	// Input Actions
	void UpdateActions();

	map<string,InputAction> m_inputActions;
	map<Keyboard::Keys, InputAction> m_keyMappings;

	void RegisterInputActions();
	void RegisterInputAction(InputAction action);

	void MapKeys();
	void MapKey(Keyboard::Keys key, string action);
};

