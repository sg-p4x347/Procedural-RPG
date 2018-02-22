#include "pch.h"
#include "PlayerSystem.h"
#include "Game.h"
#include "IEventManager.h"
PlayerSystem::PlayerSystem(
	SystemManager * systemManager,
	unique_ptr<WorldEntityManager> &  entityManager,
	vector<string> & components, 
	unsigned short updatePeriod
) : 
WorldSystem::WorldSystem(entityManager,components,updatePeriod),
SM(systemManager)
{
}



string PlayerSystem::Name()
{
	return "Player";
}

void PlayerSystem::Update(double & elapsed)
{
	shared_ptr<Components::Position> position = EM->Player()->GetComponent<Components::Position>("Position");
	shared_ptr<Components::Movement> movement = EM->Player()->GetComponent<Components::Movement>("Movement");
	shared_ptr<Components::Player> playerComp = EM->Player()->GetComponent<Components::Player>("Player");
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

	SimpleMath::Vector3 input = SimpleMath::Vector3::Zero;
	if (keyboardState.Up || keyboardState.W)
		input.z += 1.f;

	if (keyboardState.Down || keyboardState.S)
		input.z -= 1.f;

	if (keyboardState.Left || keyboardState.A)
		input.x += 1.f;

	if (keyboardState.Right || keyboardState.D)
		input.x -= 1.f;

	

	
	// adjust the velocity according to orientation
	Vector3 velocity;
	switch (playerComp->MovementState) {
	case Components::MovementStates::Normal:
		input.Normalize();
		velocity = SimpleMath::Vector3::Transform(input, GetPlayerQuaternion(true)) * (keyboardState.LeftShift ? 8.f : 5.f);
		velocity.y = movement->Velocity.y;
		
		movement->Velocity = velocity;
		break;
	case Components::MovementStates::Spectator:
		if (keyboardState.PageUp || keyboardState.Space)
			input.y += 1.f;

		if (keyboardState.PageDown || keyboardState.LeftShift)
			input.y -= 1.f;
		input.Normalize();
		movement->Velocity = SimpleMath::Vector3::Transform(input, GetPlayerQuaternion()) * (keyboardState.LeftControl ? 1000 : 100);
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
	SetMovementToSpectator();
}

Quaternion PlayerSystem::GetPlayerQuaternion(bool ignorePitch)
{
	auto position = EM->PlayerPos();
	return SimpleMath::Quaternion::CreateFromYawPitchRoll(position->Rot.x, ignorePitch  ? 0.f : -position->Rot.y, 0.f);
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
