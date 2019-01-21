#include "pch.h"
#include "PlayerSystem.h"
#include "Game.h"
#include "IEventManager.h"
#include "Inventory.h"
#include "GuiSystem.h"
#include "AssetManager.h"
#include "TaskManager.h"
// TEMP
#include "BuildingSystem.h"
#include "TerrainSystem.h"
namespace world {
	PlayerSystem::PlayerSystem(
		SystemManager * systemManager,
		WEM *  entityManager,
		unsigned short updatePeriod
	) :
		WorldSystem::WorldSystem(entityManager, updatePeriod),
		SM(systemManager)
	{
		//if(EM->GetEntity<world::Player>(EM->PlayerID()) == nullptr) {
		//	CreatePlayer();
		//}
		RegisterInputActions();
		MapKeys();
		RegisterHandlers();
		CreatePlayer();
	}



	string PlayerSystem::Name()
	{
		return "Player";
	}

	void PlayerSystem::Update(double & elapsed)
	{
		MaskType queryMask = EM->PlayerSignature();
		MaskType readMask = EM->GetMask<Position, Movement, Collision, Player,MovementController>();
		MaskType writeMask = EM->GetMask<Movement, Collision, Player, MovementController>();
		TaskManager::Get().RunSynchronous(Task([=] {
			auto player = EM->GetEntity<Position, Movement, Player,MovementController>(EM->PlayerID());
			Position & position = player->Get<Position>();
			Movement & movement = player->Get<Movement>();
			Player & playerComp = player->Get<Player>();
			MovementController & movementController = player->Get<MovementController>();
			// mouse
			auto mouseState = Game::MouseState;
			auto keyboardState = Game::KeyboardState;
			static int sampleCounter = 0;
			if (mouseState.positionMode == Mouse::MODE_RELATIVE) {
				Vector2 delta = Vector2(mouseState.x, mouseState.y);


				//SimpleMath::Vector2 delta = SimpleMath::Vector2(float(mouseState.x), float(mouseState.y));
				static const float X_MOUSE_GAIN = 10.f;
				static const float Y_MOUSE_GAIN = 0.1f;
				/*if (movement->AngularVelocity.y > 0) {
					movement->AngularVelocity.y -= 0.5;
				}*/
				/*if (delta.x != 0.f || delta.y != 0.f || sampleCounter++ > 3) {
					movement->AngularVelocity.x = -delta.x * elapsed * MOUSE_GAIN;
					movement->AngularVelocity.y = -delta.y * elapsed * MOUSE_GAIN;
					sampleCounter = 0;
				}*/
				Vector2 mousePos = Game::Get().MousePos;
				float length = mousePos.Length();
				if (length != 0.f)
					SM->GetSystem<GuiSystem>("Gui")->SetTextByID("Output1", to_string(length));

				playerComp.CameraPitch -= mousePos.y * elapsed * Y_MOUSE_GAIN;
				movement.AngularVelocity.y = (movement.AngularVelocity.y - mousePos.x * elapsed * X_MOUSE_GAIN) / 2.f;
				//if (std::abs(movement.AngularVelocity.y) < 0.1f) movement.AngularVelocity.y = 0.f;
				Game::Get().MousePos = Vector2::Zero;
				//AccumulatedMousePos = Vector2::Zero;
				//Game::Get().MousePos = Vector2::Zero;

				// limit pitch to straight up or straight down
				// with a little fudge-factor to avoid gimbal lock
				playerComp.CameraPitch = std::min(XM_PIDIV2 - 0.01f, std::max(-XM_PIDIV2 + 0.01f, playerComp.CameraPitch));
			}
			//----------------------------------------------------------------
			// Directional input
			m_direction = Vector3::Zero;

			UpdateActions();



			// adjust the velocity according to orientation
			Vector3 heading;
			switch (movementController.Type) {
			case MovementController::MovementTypes::Normal:
				movementController.Heading = SimpleMath::Vector3::Transform(m_direction, GetPlayerQuaternion(playerComp, position, true));
				movementController.Heading.Normalize();
				movementController.Jump = m_direction.y > 0.f;
				movementController.Speed = (keyboardState.LeftControl ? 0.2f : keyboardState.LeftShift ? 8.f : 5.f);
				break;
			case MovementController::MovementTypes::Spectator:
				movementController.Heading = SimpleMath::Vector3::Transform(m_direction, GetPlayerQuaternion(playerComp, position));
				movementController.Heading.Normalize();
				movementController.Speed = (keyboardState.LeftControl ? 1.f : keyboardState.LeftShift ? 1000 : 20);
				break;
			}

			//----------------------------------------------------------------
			// Keyboard events


			if (Game::Get().KeyboardTracker.IsKeyPressed(DirectX::Keyboard::Keys::E)) {
				//SM->GetSystem<ActionSystem>("Action")->Check();
				//SM->GetEventManager().Invoke("InvokeAction");
				IEventManager::Invoke(EventTypes::Action_Check);
			}
			if (Game::Get().KeyboardTracker.IsKeyPressed(DirectX::Keyboard::Keys::B)) {
				Utility::OutputLine("Generating Buildings...");
				Rectangle footprint = Rectangle(position.Pos.x,position.Pos.z, 4, 6);
				Rectangle flattenArea = Rectangle(footprint.x - 1, footprint.y - 1, footprint.width + 2, footprint.height + 2);

				float height = SM->GetSystem<TerrainSystem>("Terrain")->Flatten(flattenArea, 10);
				SM->GetSystem<BuildingSystem>("Building")->CreateAdobe(Vector3(footprint.x, height + 0.2f, footprint.y), Rectangle(0, 0, footprint.width, footprint.height));
			}

			// Toggle movement state
			if (Game::Get().KeyboardTracker.IsKeyPressed(DirectX::Keyboard::Keys::R)) {
				switch (movementController.Type) {
				case MovementController::MovementTypes::Normal:
					SetMovementToSpectator();
					break;
				case MovementController::MovementTypes::Spectator:
					SetMovementToNormal();
					break;
				}
			}
		},
		queryMask,
		readMask,
		writeMask));
	}

	//void PlayerSystem::SetMousePos(Vector2 pos)
	//{

	//	//Movement movement = EM->Player()->GetComponent < Components::Movement>("Movement");
	//	//shared_ptr<Components::Position> position = EM->Player()->GetComponent < Components::Position>("Position");
	//	//if (std::abs(pos.y) != 0)
	//	//position->Rot.y -= pos.y * 0.0016;
	//	//movement->AngularVelocity.y = -pos.y * 0.16;
	//	//movement->AngularVelocity.x = -pos.x * 0.16;
	//	//if (std::abs(pos.x) != 0)
	//	//position->Rot.x -= std::min(pos.x,1.f) * 0.0016;
	//	//position->Rot.y -= std::min(pos.y, 1.f) * 0.0016;
	//	//AccumulatedMousePos += pos;
	//	/*static std::chrono::milliseconds last;
	//	auto now = std::chrono::duration_cast< std::chrono::milliseconds >(
	//		std::chrono::system_clock::now().time_since_epoch()
	//		);
	//	SM->GetSystem<GuiSystem>("Gui")->SetTextByID("Output1", to_string((now - last).count()));
	//	last = now;*/
	//	/*if (std::abs(pos.x) == 0 && std::abs(pos.y) == 0) {
	//		auto test = 0;
	//	}*/

	//}

	void PlayerSystem::CreatePlayer()
	{
		EntityPtr asset;
		if (AssetManager::Get()->GetStaticEM()->TryFindByPathID("Biped_Placeholder", asset)) {
			EM->CreatePlayer(
				Position(),
				Player(),
				Movement(),
				MovementController(),
				Model(asset->ID(), AssetType::Authored),
				Collision(asset->ID(), AssetType::Authored)
			);
		}
		
		SetMovementToSpectator();
	}

	Quaternion PlayerSystem::GetPlayerQuaternion(world::Player & player, world::Position & position, bool ignorePitch)
	{
		return SimpleMath::Quaternion::CreateFromYawPitchRoll(position.Rot.y, ignorePitch ? 0.f : - player.CameraPitch, 0.f);
	}

	void PlayerSystem::Run()
	{
		System::Run();
		auto player = EM->GetEntity<Player>(EM->PlayerID());
		if (!player) throw exception("Player was null");
		SetInteractionState(player->Get<Player>().InteractionState);
		Game::Get().MousePos = Vector2::Zero;
	}

	void PlayerSystem::RegisterHandlers()
	{
		IEventManager::RegisterHandler(EventTypes::Player_SetInteractionState, std::function<void(Player::InteractionStates)>([this](Player::InteractionStates state) {
			SetInteractionState(state);
		}));
		IEventManager::RegisterHandler(EventTypes::Item_OpenInventory, std::function<void(EntityID, EntityID)>([this](EntityID actor, EntityID target) {
			EM->GetEntity<Player>(actor)->Get<Player>().OpenContainer = target;
			SetInteractionState(Player::InteractionStates::InventoryUI);
		}));
	}

	Player & PlayerSystem::GetPlayerComp()
	{
		return EM->GetEntity<Player>(EM->PlayerID())->Get<Player>();
	}

	void PlayerSystem::SetMovementToNormal()
	{
		auto player = EM->GetEntity<Collision, Movement, MovementController>(EM->PlayerID());
		player->Get<Collision>().Enabled = true;
		player->Get<Movement>().Acceleration.y = -9.8f;
		player->Get<MovementController>().Type = MovementController::MovementTypes::Normal;
	}

	void PlayerSystem::SetMovementToSpectator()
	{
		auto player = EM->GetEntity<Collision, Movement, MovementController>(EM->PlayerID());
		player->Get<Collision>().Enabled = false;
		player->Get<Movement>().Acceleration.y = 0.f;
		player->Get<MovementController>().Type = MovementController::MovementTypes::Spectator;
	}

	void PlayerSystem::SetInteractionState(Player::InteractionStates state)
	{
		Player & playerComp = EM->GetEntity<Player>(EM->PlayerID())->Get<Player>();
		playerComp.InteractionState = state;

		//EntityPtr openContainer;
		switch (state) {
		case Player::InteractionStates::ThirdPerson:
		case Player::InteractionStates::FirstPerson:
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
		Player & playerComp = EM->GetEntity<Player>(EM->PlayerID())->Get<Player>();
		for (auto & keyMapping : m_keyMappings) {
			if (Game::Get().KeyboardState.IsKeyDown(keyMapping.first)) {
				// Key pressed
				if (keyMapping.second.EdgeTrigger) {
					// Only invoke the action on an edge
					if (!keyMapping.second.Registered) {
						// Key just now pressed
						if (!keyMapping.second.WorldOnly || (playerComp.InteractionState == Player::InteractionStates::FirstPerson || playerComp.InteractionState == Player::InteractionStates::ThirdPerson))
							keyMapping.second.Action();
						// register the press
						keyMapping.second.Registered = true;
					}
				}
				else {
					// Invoke the action continuously
					if (!keyMapping.second.WorldOnly || (playerComp.InteractionState == Player::InteractionStates::FirstPerson || playerComp.InteractionState == Player::InteractionStates::ThirdPerson))
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
			Player & playerComp = EM->GetEntity<Player>(EM->PlayerID())->Get<Player>();
			if (playerComp.InteractionState == Player::InteractionStates::InventoryUI) {
				SetInteractionState(Player::InteractionStates::FirstPerson);
			}
			else {
				SetInteractionState(Player::InteractionStates::InventoryUI);
			}
		}, true, false));
		//----------------------------------------------------------------
		// Camera
		RegisterInputAction(InputAction("ToggleFirstPerson", "Toggle between first and third person mode", [this] {
			Player & playerComp = EM->GetEntity<Player>(EM->PlayerID())->Get<Player>();
			SetInteractionState(playerComp.InteractionState == Player::InteractionStates::FirstPerson ? 
				Player::InteractionStates::ThirdPerson : 
				Player::InteractionStates::FirstPerson);

		}, true, true));

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
		//----------------------------------------------------------------
		// Camera
		MapKey(Keyboard::Keys::F1, "ToggleFirstPerson");
	}

	void PlayerSystem::MapKey(Keyboard::Keys key, string action)
	{
		m_keyMappings.insert(std::make_pair(key, m_inputActions[action]));
	}
}