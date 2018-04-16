#include "pch.h"
#include "PlayerSystem.h"
#include "Game.h"
#include "IEventManager.h"
#include "Inventory.h"
PlayerSystem::PlayerSystem(
	SystemManager * systemManager,
	unique_ptr<WorldEntityManager> &  entityManager,
	vector<string> & components, 
	unsigned short updatePeriod
) : 
WorldSystem::WorldSystem(entityManager,components,updatePeriod),
SM(systemManager)
{
	RegisterInputActions();
	MapKeys();
	RegisterHandlers();
}



string PlayerSystem::Name()
{
	return "Player";
}

void PlayerSystem::Update(double & elapsed)
{
	shared_ptr<Components::Position> position = EM->Player()->GetComponent<Components::Position>("Position");
	shared_ptr<Components::Movement> movement = EM->Player()->GetComponent<Components::Movement>("Movement");
	shared_ptr<Components::Player> playerComp = GetPlayerComp();
	// mouse
	auto mouseState = Game::MouseState;
	auto keyboardState = Game::KeyboardState;
	if (mouseState.positionMode == Mouse::MODE_RELATIVE) {
		SimpleMath::Vector2 delta = SimpleMath::Vector2(float(mouseState.x), float(mouseState.y))*2.f;
		static const float MOUSE_GAIN = 1.f;
		//movement->AngularVelocity.y = -delta.y * MOUSE_GAIN;
		//movement->AngularVelocity.x = -delta.x * MOUSE_GAIN;
		position->Rot.y -= delta.y * elapsed;
		position->Rot.x -= delta.x * elapsed;

		// limit pitch to straight up or straight down
		// with a little fudge-factor to avoid gimbal lock
		float limit = XM_PI / 2.0f - 0.01f;
		position->Rot.y = std::max(-limit, position->Rot.y);
		position->Rot.y = std::min(limit, position->Rot.y);

		// keep longitude in sane range by wrapping
		if (position->Rot.x > XM_PI)
		{
			position->Rot.x -= XM_PI * 2.0f;
		}
		else if (position->Rot.x < -XM_PI)
		{
			position->Rot.x += XM_PI * 2.0f;
		}
	}
	//----------------------------------------------------------------
	// Directional input
	m_direction = Vector3::Zero;

	
	UpdateActions();

	/*SimpleMath::Vector3 input = SimpleMath::Vector3::Zero;
	if (keyboardState.Up || keyboardState.W)
		input.z += 1.f;

	if (keyboardState.Down || keyboardState.S)
		input.z -= 1.f;

	if (keyboardState.Left || keyboardState.A)
		input.x += 1.f;

	if (keyboardState.Right || keyboardState.D)
		input.x -= 1.f;*/

	

	
	// adjust the velocity according to orientation
	Vector3 velocity;
	switch (playerComp->MovementState) {
	case Components::MovementStates::Normal:
		m_direction.y = 0.f;
		m_direction.Normalize();
		velocity = SimpleMath::Vector3::Transform(m_direction, GetPlayerQuaternion(true)) * (keyboardState.LeftShift ? 8.f : 5.f);
		velocity.y = movement->Velocity.y;
		
		movement->Velocity = velocity;
		break;
	case Components::MovementStates::Spectator:
		/*if (keyboardState.PageUp || keyboardState.Space)
			input.y += 1.f;

		if (keyboardState.PageDown || keyboardState.LeftShift)
			input.y -= 1.f;*/
		m_direction.Normalize();
		movement->Velocity = SimpleMath::Vector3::Transform(m_direction, GetPlayerQuaternion()) * (keyboardState.LeftControl ? 1000 : 100);
		break;
	}

	//----------------------------------------------------------------
	// Keyboard events


	if (Game::Get().KeyboardTracker.IsKeyPressed(DirectX::Keyboard::Keys::E)) {
		//SM->GetSystem<ActionSystem>("Action")->Check();
		//SM->GetEventManager().Invoke("InvokeAction");
		IEventManager::Invoke(EventTypes::Action_Check);
	}

	// Toggle movement state
	if (Game::Get().KeyboardTracker.IsKeyPressed(DirectX::Keyboard::Keys::R)) {
		switch (playerComp->MovementState) {
		case Components::MovementStates::Normal:
			SetMovementToSpectator();
			break;
		case Components::MovementStates::Spectator:
			SetMovementToNormal();
			break;
		}
	}
}

void PlayerSystem::CreatePlayer()
{
	EntityPtr player = EM->NewEntity();
	player->AddComponent(new Components::Player());
	auto pos = new Components::Position();
	pos->Pos.y = 10;
	player->AddComponent(pos);
	player->AddComponent(new Components::Movement());
	player->AddComponent(new Components::Collision(Box(Vector3(-0.25f,-1.7f,-0.25f),Vector3(0.5f,1.7f,0.5f))));
	player->AddComponent(new Components::Inventory(50.f,50.f));
	SetMovementToSpectator();
}

Quaternion PlayerSystem::GetPlayerQuaternion(bool ignorePitch)
{
	auto position = EM->PlayerPos();
	return SimpleMath::Quaternion::CreateFromYawPitchRoll(position->Rot.x, ignorePitch  ? 0.f : -position->Rot.y, 0.f);
}

void PlayerSystem::Run()
{
	System::Run();
	SetInteractionState(GetPlayerComp()->InteractionState);
}

void PlayerSystem::RegisterHandlers()
{
	IEventManager::RegisterHandler(EventTypes::Player_SetInteractionState, std::function<void(Components::InteractionStates)>([this](Components::InteractionStates state) {
		SetInteractionState(state);
	}));
	IEventManager::RegisterHandler(EventTypes::Item_OpenInventory, std::function<void(EntityPtr,EntityPtr)>([this](EntityPtr actor, EntityPtr target) {
		actor->GetComponent<Components::Player>("Player")->OpenContainer = target->ID();
		SetInteractionState(Components::InteractionStates::InventoryUI);
	}));
}

shared_ptr<Components::Player> PlayerSystem::GetPlayerComp()
{
	return EM->Player()->GetComponent<Components::Player>("Player");
}

void PlayerSystem::SetMovementToNormal()
{
	EM->Player()->GetComponent<Components::Player>("Player")->MovementState = Components::MovementStates::Normal;
	EM->Player()->GetComponent<Components::Movement>("Movement")->Acceleration.y = -9.8f;
	EM->Player()->GetComponent<Components::Collision>("Collision")->Enabled = true;
}

void PlayerSystem::SetMovementToSpectator()
{
	EM->Player()->GetComponent<Components::Player>("Player")->MovementState = Components::MovementStates::Spectator;
	EM->Player()->GetComponent<Components::Movement>("Movement")->Acceleration.y = 0.f;
	EM->Player()->GetComponent<Components::Collision>("Collision")->Enabled = false;
}

void PlayerSystem::SetInteractionState(Components::InteractionStates state)
{
	auto playerComp = GetPlayerComp();
	playerComp->InteractionState = state;

	//EntityPtr openContainer;
	switch (state) {
	case Components::InteractionStates::World:
		Mouse::Get().SetMode(Mouse::MODE_RELATIVE);

		EM->Player()->GetComponent<Components::Player>("Player")->OpenContainer = 0;
		//IEventManager::Invoke(EventTypes::Item_CloseInventory, EM->Player(),openContainer);
		IEventManager::Invoke(EventTypes::GUI_OpenMenu, string("HUD"));
		break;
	case Components::InteractionStates::InventoryUI:
	case Components::InteractionStates::ContainerUI:
		Mouse::Get().SetMode(Mouse::MODE_ABSOLUTE);
		//EM->Find(EM->Player()->GetComponent<Components::Player>("Player")->OpenContainer, openContainer);
		//IEventManager::Invoke(EventTypes::Item_OpenInventory, EM->Player(),(openContainer ? openContainer : EM->Player()));
		IEventManager::Invoke(EventTypes::GUI_OpenMenu, string("inventory"));
		
		break;
	case Components::InteractionStates::GamePausedUI:
		Mouse::Get().SetMode(Mouse::MODE_ABSOLUTE);
		IEventManager::Invoke(EventTypes::GUI_OpenMenu, string("game_paused"));
		break;
	}
}

void PlayerSystem::UpdateActions()
{
	auto playerComp = GetPlayerComp();
	for (auto & keyMapping : m_keyMappings) {
		if (Game::Get().KeyboardState.IsKeyDown(keyMapping.first)) {
			// Key pressed
			if (keyMapping.second.EdgeTrigger) {
				// Only invoke the action on an edge
				if (!keyMapping.second.Registered) {
					// Key just now pressed
					if (!keyMapping.second.WorldOnly || playerComp->InteractionState == Components::InteractionStates::World)
						keyMapping.second.Action();
					// register the press
					keyMapping.second.Registered = true;
				}
			}
			else {
				// Invoke the action continuously
				if (!keyMapping.second.WorldOnly || playerComp->InteractionState == Components::InteractionStates::World)
					keyMapping.second.Action();
			}
		}
		else {
			// Key released
			if (keyMapping.second.EdgeTrigger) {
				// Reset the key-pressed flag
				if (keyMapping.second.Registered) {
					// Key just now released
					keyMapping.second.Registered = false;
				}
			}

		}
	}
}

void PlayerSystem::RegisterInputActions()
{
	//----------------------------------------------------------------
	// Movement
	RegisterInputAction(InputAction("Forward", "Move forward", [this] {
		m_direction.z += 1.f;
	}));
	RegisterInputAction(InputAction("Back", "Move backward", [this] {
		m_direction.z -= 1.f;
	}));
	RegisterInputAction(InputAction("Left", "Strafe left", [this] {
		m_direction.x += 1.f;
	}));
	RegisterInputAction(InputAction("Right", "Strafe right", [this] {
		m_direction.x -= 1.f;
	}));
	RegisterInputAction(InputAction("Up", "Move up", [this] {
		m_direction.y += 1.f;
	}));
	RegisterInputAction(InputAction("Down", "Move down", [this] {
		m_direction.y -= 1.f;
	}));
	//----------------------------------------------------------------
	// Inventory
	RegisterInputAction(InputAction("ToggleInventory", "Toggle inventory", [this] {
		if (EM->Player()->GetComponent<Components::Player>("Player")->InteractionState == Components::InteractionStates::InventoryUI) {
			SetInteractionState(Components::InteractionStates::World);
		} else {
			SetInteractionState(Components::InteractionStates::InventoryUI);
		}
	},true,false));

}

void PlayerSystem::RegisterInputAction(InputAction action)
{
	m_inputActions.insert(std::make_pair(action.Name, action));
}

void PlayerSystem::MapKeys()
{
	//----------------------------------------------------------------
	// Movement
	MapKey(Keyboard::Keys::W, "Forward");
	MapKey(Keyboard::Keys::S, "Back");
	MapKey(Keyboard::Keys::D, "Right");
	MapKey(Keyboard::Keys::A, "Left");
	MapKey(Keyboard::Keys::Space, "Up");
	MapKey(Keyboard::Keys::LeftShift, "Down");
	//----------------------------------------------------------------
	// Inventory
	MapKey(Keyboard::Keys::Tab, "ToggleInventory");
}

void PlayerSystem::MapKey(Keyboard::Keys key, string action)
{
	m_keyMappings.insert(std::make_pair(key, m_inputActions[action]));
}
