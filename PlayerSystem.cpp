#include "pch.h"
#include "PlayerSystem.h"
#include "Game.h"
#include "IEventManager.h"
#include "Inventory.h"
#include "GuiSystem.h"
#include "AssetManager.h"
namespace world {
	PlayerSystem::PlayerSystem(
		SystemManager * systemManager,
		WEM *  entityManager,
		unsigned short updatePeriod
	) :
		WorldSystem::WorldSystem(entityManager, updatePeriod),
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
		auto player = EM->GetEntity<Position, Movement, Player>(EM->PlayerID());
		Position & position = player.Get<Position>();
		Movement & movement = player.Get<Movement>();
		Player & playerComp = player.Get<Player>();
		// mouse
		auto mouseState = Game::MouseState;
		auto keyboardState = Game::KeyboardState;
		static int sampleCounter = 0;
		if (mouseState.positionMode == Mouse::MODE_RELATIVE) {
			Vector2 delta = Vector2(mouseState.x, mouseState.y);


			//SimpleMath::Vector2 delta = SimpleMath::Vector2(float(mouseState.x), float(mouseState.y));
			static const float MOUSE_GAIN = 0.1f;
			/*if (movement->AngularVelocity.y > 0) {
				movement->AngularVelocity.y -= 0.5;
			}*/
			/*if (delta.x != 0.f || delta.y != 0.f || sampleCounter++ > 3) {
				movement->AngularVelocity.x = -delta.x * elapsed * MOUSE_GAIN;
				movement->AngularVelocity.y = -delta.y * elapsed * MOUSE_GAIN;
				sampleCounter = 0;
			}*/
			float length = AccumulatedMousePos.Length();
			if (length != 0.f)
				SM->GetSystem<GuiSystem>("Gui")->SetTextByID("Output1", to_string(length));

			playerComp.CameraPitch -= AccumulatedMousePos.y * elapsed * MOUSE_GAIN;
			position.Rot.y -= AccumulatedMousePos.x * elapsed * MOUSE_GAIN;
			AccumulatedMousePos = Vector2::Zero;
			//Game::Get().MousePos = Vector2::Zero;

			// limit pitch to straight up or straight down
			// with a little fudge-factor to avoid gimbal lock
			playerComp.CameraPitch = std::min(XM_PIDIV2 - 0.01f, std::max(-XM_PIDIV2 + 0.01f, playerComp.CameraPitch));
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
		switch (playerComp.MovementState) {
		case Player::MovementStates::Normal:
			m_direction.y = 0.f;
			m_direction.Normalize();
			velocity = SimpleMath::Vector3::Transform(m_direction, GetPlayerQuaternion(playerComp,position,true)) * (keyboardState.LeftControl ? 0.1 : keyboardState.LeftShift ? 8.f : 5.f);
			velocity.y = movement.Velocity.y;

			movement.Velocity = velocity;
			break;
		case Player::MovementStates::Spectator:
			/*if (keyboardState.PageUp || keyboardState.Space)
				input.y += 1.f;

			if (keyboardState.PageDown || keyboardState.LeftShift)
				input.y -= 1.f;*/
			m_direction.Normalize();
			movement.Velocity = SimpleMath::Vector3::Transform(m_direction, GetPlayerQuaternion(playerComp, position)) * (keyboardState.LeftControl ? 0.1f : keyboardState.LeftShift ? 1000 : 100);
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
			switch (playerComp.MovementState) {
			case Player::MovementStates::Normal:
				SetMovementToSpectator();
				break;
			case Player::MovementStates::Spectator:
				SetMovementToNormal();
				break;
			}
		}
	}

	void PlayerSystem::SetMousePos(Vector2 pos)
	{

		//Movement movement = EM->Player()->GetComponent < Components::Movement>("Movement");
		//shared_ptr<Components::Position> position = EM->Player()->GetComponent < Components::Position>("Position");
		//if (std::abs(pos.y) != 0)
		//position->Rot.y -= pos.y * 0.0016;
		//movement->AngularVelocity.y = -pos.y * 0.16;
		//movement->AngularVelocity.x = -pos.x * 0.16;
		//if (std::abs(pos.x) != 0)
		//position->Rot.x -= std::min(pos.x,1.f) * 0.0016;
		//position->Rot.y -= std::min(pos.y, 1.f) * 0.0016;
		AccumulatedMousePos += pos;
		/*static std::chrono::milliseconds last;
		auto now = std::chrono::duration_cast< std::chrono::milliseconds >(
			std::chrono::system_clock::now().time_since_epoch()
			);
		SM->GetSystem<GuiSystem>("Gui")->SetTextByID("Output1", to_string((now - last).count()));
		last = now;*/
		/*if (std::abs(pos.x) == 0 && std::abs(pos.y) == 0) {
			auto test = 0;
		}*/

	}

	void PlayerSystem::CreatePlayer()
	{
		Model model = Model();
		EntityPtr asset;
		if (AssetManager::Get()->GetStaticEM()->TryFindByPathID("Biped_Placeholder", asset)) {
			model.Asset = asset->ID();
			model.Type = AssetType::Authored;
		}
		EM->CreatePlayer(
			Position(), 
			Player(),
			Movement(), 
			Collision(Box(Vector3(0.f, 0.85f, 0.f), Vector3(0.5f, 1.7f, 0.25f))), 
			Inventory(50.f,50.f),
			std::move(model)
		);
		SetMovementToSpectator();
	}

	Quaternion PlayerSystem::GetPlayerQuaternion(world::Player & player, world::Position & position, bool ignorePitch)
	{
		return SimpleMath::Quaternion::CreateFromYawPitchRoll(position.Rot.y, ignorePitch ? 0.f : - player.CameraPitch, 0.f);
	}

	void PlayerSystem::Run()
	{
		System::Run();
		SetInteractionState(EM->GetEntity<Player>(EM->PlayerID()).Get<Player>().InteractionState);
	}

	void PlayerSystem::RegisterHandlers()
	{
		IEventManager::RegisterHandler(EventTypes::Player_SetInteractionState, std::function<void(Player::InteractionStates)>([this](Player::InteractionStates state) {
			SetInteractionState(state);
		}));
		IEventManager::RegisterHandler(EventTypes::Item_OpenInventory, std::function<void(EntityID, EntityID)>([this](EntityID actor, EntityID target) {
			EM->GetEntity<Player>(actor).Get<Player>().OpenContainer = target;
			SetInteractionState(Player::InteractionStates::InventoryUI);
		}));
	}

	Player & PlayerSystem::GetPlayerComp()
	{
		return EM->GetEntity<Player>(EM->PlayerID()).Get<Player>();
	}

	void PlayerSystem::SetMovementToNormal()
	{
		auto player = EM->GetEntity<Collision, Movement, Player>(EM->PlayerID());
		player.Get<Collision>().Enabled = true;
		player.Get<Movement>().Acceleration.y = -9.8f;
		player.Get<Player>().MovementState = Player::MovementStates::Normal;
	}

	void PlayerSystem::SetMovementToSpectator()
	{
		auto player = EM->GetEntity<Collision, Movement, Player>(EM->PlayerID());
		player.Get<Collision>().Enabled = false;
		player.Get<Movement>().Acceleration.y = 0.f;
		player.Get<Player>().MovementState = Player::MovementStates::Spectator;
	}

	void PlayerSystem::SetInteractionState(Player::InteractionStates state)
	{
		Player & playerComp = EM->GetEntity<Player>(EM->PlayerID()).Get<Player>();
		playerComp.InteractionState = state;

		//EntityPtr openContainer;
		switch (state) {
		case Player::InteractionStates::World:
			Mouse::Get().SetMode(Mouse::MODE_RELATIVE);
			playerComp.OpenContainer = 0;
			//IEventManager::Invoke(EventTypes::Item_CloseInventory, EM->Player(),openContainer);
			IEventManager::Invoke(EventTypes::GUI_OpenMenu, string("HUD"));
			break;
		case Player::InteractionStates::InventoryUI:
		case Player::InteractionStates::ContainerUI:
			Mouse::Get().SetMode(Mouse::MODE_ABSOLUTE);
			//EM->Find(EM->Player()->GetComponent<Components::Player>("Player")->OpenContainer, openContainer);
			//IEventManager::Invoke(EventTypes::Item_OpenInventory, EM->Player(),(openContainer ? openContainer : EM->Player()));
			IEventManager::Invoke(EventTypes::GUI_OpenMenu, string("InventoryGui"));

			break;
		case Player::InteractionStates::GamePausedUI:
			Mouse::Get().SetMode(Mouse::MODE_ABSOLUTE);
			IEventManager::Invoke(EventTypes::GUI_OpenMenu, string("game_paused"));
			break;
		}
	}

	void PlayerSystem::UpdateActions()
	{
		Player & playerComp = EM->GetEntity<Player>(EM->PlayerID()).Get<Player>();
		for (auto & keyMapping : m_keyMappings) {
			if (Game::Get().KeyboardState.IsKeyDown(keyMapping.first)) {
				// Key pressed
				if (keyMapping.second.EdgeTrigger) {
					// Only invoke the action on an edge
					if (!keyMapping.second.Registered) {
						// Key just now pressed
						if (!keyMapping.second.WorldOnly || playerComp.InteractionState == Player::InteractionStates::World)
							keyMapping.second.Action();
						// register the press
						keyMapping.second.Registered = true;
					}
				}
				else {
					// Invoke the action continuously
					if (!keyMapping.second.WorldOnly || playerComp.InteractionState == Player::InteractionStates::World)
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
			Player & playerComp = EM->GetEntity<Player>(EM->PlayerID()).Get<Player>();
			if (playerComp.InteractionState == Player::InteractionStates::InventoryUI) {
				SetInteractionState(Player::InteractionStates::World);
			}
			else {
				SetInteractionState(Player::InteractionStates::InventoryUI);
			}
		}, true, false));

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
		//MapKey(Keyboard::Keys::LeftShift, "Down");
		//----------------------------------------------------------------
		// Inventory
		MapKey(Keyboard::Keys::Tab, "ToggleInventory");
	}

	void PlayerSystem::MapKey(Keyboard::Keys key, string action)
	{
		m_keyMappings.insert(std::make_pair(key, m_inputActions[action]));
	}
}