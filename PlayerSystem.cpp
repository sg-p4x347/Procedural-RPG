#include "pch.h"
#include "PlayerSystem.h"
#include "Position.h"
#include "Player.h"
#include "Movement.h";

PlayerSystem::PlayerSystem(
	shared_ptr<EntityManager> & entityManager, 
	vector<string> & components, 
	unsigned short updatePeriod,
	shared_ptr<DirectX::Mouse> mouse,
	shared_ptr<DirectX::Keyboard> keyboard
) : System::System(
	entityManager,
	components,
	updatePeriod
	),
	m_mouse(mouse),
	m_keyboard(keyboard)
{
}



string PlayerSystem::Name()
{
	return "Player";
}

void PlayerSystem::Update(double & elapsed)
{
	shared_ptr<Components::Position> position = EM->GetComponent<Components::Position>(EM->Player(),"Position");
	shared_ptr<Components::Movement> movement = EM->GetComponent<Components::Movement>(EM->Player(), "Movement");

	// mouse
	auto mouseState = m_mouse->GetState();
	auto keyboardState = m_keyboard->GetState();
	if (mouseState.positionMode == Mouse::MODE_RELATIVE) {
		SimpleMath::Vector2 delta = SimpleMath::Vector2(float(mouseState.x), float(mouseState.y))*2.f;

		movement->AngularVelocity.y = -delta.y;
		movement->AngularVelocity.x = -delta.x;
		//position->Rot.y -= delta.y;
		//position->Rot.x -= delta.x;

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
	shared_ptr<Entity> player = EM->NewEntity();
	player->AddComponent(EM->ComponentMask("Player"), shared_ptr<Components::Player>(new Components::Player(player->ID())));
	shared_ptr<Components::Position> pos(new Components::Position(player->ID()));
	pos->Pos.y = 10;
	player->AddComponent(EM->ComponentMask("Position"),pos);
	player->AddComponent(EM->ComponentMask("Movement"), shared_ptr<Components::Movement>(new Components::Movement(player->ID())));
}
