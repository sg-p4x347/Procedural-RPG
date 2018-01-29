#include "pch.h"
#include "PlayerSystem.h"
#include "Position.h"
#include "Player.h"
#include "Movement.h";
#include "Game.h"

PlayerSystem::PlayerSystem(
	unique_ptr<WorldEntityManager> &  entityManager,
	vector<string> & components, 
	unsigned short updatePeriod
) : WorldSystem::WorldSystem(
	entityManager,
	components,
	updatePeriod
	)
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

	// mouse
	auto mouseState = Game::MouseState;
	auto keyboardState = Game::KeyboardState;
	if (mouseState.positionMode == Mouse::MODE_RELATIVE) {
		SimpleMath::Vector2 delta = SimpleMath::Vector2(float(mouseState.x), float(mouseState.y))*2.f;
		static const float MOUSE_GAIN = 1.f;
		movement->AngularVelocity.y = -delta.y * MOUSE_GAIN;
		movement->AngularVelocity.x = -delta.x * MOUSE_GAIN;
		//position->Rot.y -= delta.y * 0.004f;
		//position->Rot.x -= delta.x * 0.004f;

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
	// keyboard
	SimpleMath::Vector3 input = SimpleMath::Vector3::Zero;
	if (keyboardState.Up || keyboardState.W)
		input.z += 1.f;

	if (keyboardState.Down || keyboardState.S)
		input.z -= 1.f;

	if (keyboardState.Left || keyboardState.A)
		input.x += 1.f;

	if (keyboardState.Right || keyboardState.D)
		input.x -= 1.f;

	if (keyboardState.PageUp || keyboardState.Space)
		input.y += 1.f;

	if (keyboardState.PageDown || keyboardState.LeftShift)
		input.y -= 1.f;

	// adjust the velocity according to orientation
	SimpleMath::Quaternion q = SimpleMath::Quaternion::CreateFromYawPitchRoll(position->Rot.x, -position->Rot.y, 0.f);
	movement->Velocity = SimpleMath::Vector3::Transform(input, q) * (keyboardState.LeftControl ? 1000 : 100);
}

void PlayerSystem::CreatePlayer()
{
	EntityPtr player = EM->NewEntity();
	player->AddComponent(new Components::Player());
	auto pos = new Components::Position();
	pos->Pos.y = 10;
	player->AddComponent(pos);
	player->AddComponent(new Components::Movement());
}
