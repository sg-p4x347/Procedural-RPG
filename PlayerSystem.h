#pragma once
#include "WorldSystem.h"
class SystemManager;
namespace world {
	struct InputAction {
		InputAction() : InputAction::InputAction("", "", [=] {}) {}
		InputAction(string name, string description, function<void()> action, bool edgeTrigger = false, bool worldOnly = true) {
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
		PlayerSystem(
			SystemManager * systemManager, 
			WEM *  entityManager,
			unsigned short updatePeriod
		);

		// Inherited via System
		virtual string Name() override;
		virtual void Update(double & elapsed) override;
		void Run() override;
		//----------------------------------------------------------------
		// Input
		//void SetMousePos(Vector2 pos);
		//Vector2 AccumulatedMousePos;
		//----------------------------------------------------------------
		// Generation
		void CreatePlayer();

		//----------------------------------------------------------------
		// Utility
		Quaternion GetPlayerQuaternion(world::Player & player, world::Position & position, bool ignorePitch = false);
		
		//----------------------------------------------------------------
		// Interaction state
		void SetInteractionState(Player::InteractionStates state);
	private:
		SystemManager * SM;
		shared_ptr<DirectX::Mouse> m_mouse;
		shared_ptr<DirectX::Keyboard> m_keyboard;
		void RegisterHandlers();
		//----------------------------------------------------------------
		// Helpers
		Player & GetPlayerComp();
		//----------------------------------------------------------------
		// Movement
		Vector3 m_direction;
		void SetMovementToNormal();
		void SetMovementToSpectator();

		//----------------------------------------------------------------
		// Input Actions
		void UpdateActions();

		map<string, InputAction> m_inputActions;
		map<Keyboard::Keys, InputAction> m_keyMappings;

		void RegisterInputActions();
		void RegisterInputAction(InputAction action);

		void MapKeys();
		void MapKey(Keyboard::Keys key, string action);
	};
}
