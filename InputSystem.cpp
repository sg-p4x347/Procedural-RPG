#include "pch.h"
#include "InputSystem.h"

InputSystem::InputSystem(
	shared_ptr<EntityManager>& entityManager,
	vector<string>& components,
	unsigned short updatePeriod, 
	DirectX::Mouse::State mouse, 
	DirectX::Keyboard::State keyboard
) : System(entityManager,components,updatePeriod), m_mouse(mouse), m_keyboard(keyboard)
{
}

InputSystem::~InputSystem()
{
}
void InputSystem::Move(Movement & movement, Position & position) {

	Quaternion q = Quaternion::CreateFromYawPitchRoll(position.Rot.x, position.Rot.y, 0.f);
	movement.Velocity = Vector3::Transform(movement.Velocity, q);

	movement.Velocity *= 1.5;
	position.Pos += movement.Velocity;
}
void InputSystem::Stop(Movement & movement) {
	movement.Velocity = Vector3::Zero;
}
void InputSystem::Update()
{
	for (unsigned int & entity : m_entities) {
		shared_ptr<Position> position = m_entityManager->GetComponent<Position>(entity);
		shared_ptr<Movement> movement = m_entityManager->GetComponent<Movement>(entity);
		shared_ptr<Input> input = m_entityManager->GetComponent<Input>(entity);
			// mouse
			if (m_mouse.positionMode == Mouse::MODE_RELATIVE) {
				XMFLOAT3 delta = XMFLOAT3(float(m_mouse.x), float(m_mouse.y), 0.f)*0.01f;

				position->Rot.y -= delta.y;
				position->Rot.x -= delta.x;

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

		if (m_keyboard.Up || m_keyboard.W)
			movement->Velocity.x += 1.f;

		if (m_keyboard.Down || m_keyboard.S)
			movement->Velocity.z -= 1.f;

		if (m_keyboard.Left || m_keyboard.A)
			movement->Velocity.x += 1.f;

		if (m_keyboard.Right || m_keyboard.D)
			movement->Velocity.x -= 1.f;

		if (m_keyboard.PageUp || m_keyboard.Space)
			movement->Velocity.y -= 1.f;

		if (m_keyboard.PageDown || m_keyboard.LeftShift)
			movement->Velocity.y += 1.f;
		Move(*movement,*position);
	}
}
