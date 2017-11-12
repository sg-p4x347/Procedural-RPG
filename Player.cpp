#include "pch.h"
#include "Player.h"

using namespace DirectX::SimpleMath;

namespace Components {
	Components::Player::Player(const unsigned int & id) : Component::Component(id)
	{
	}

	Player::~Player()
	{
		Save(Component::GetDirectory());
	}

	string Components::Player::GetName()
	{
		return "Player";
	}
	//void Player::update(float elapsed, DirectX::Mouse::State mouse, DirectX::Keyboard::State keyboard) {
	//	// mouse
	//	if (mouse.positionMode == Mouse::MODE_RELATIVE) {
	//		XMFLOAT3 delta = XMFLOAT3(float(mouse.x), float(mouse.y), 0.f)*0.01f;

	//		rotation.y -= delta.y;
	//		rotation.x -= delta.x;

	//		// limit pitch to straight up or straight down
	//		// with a little fudge-factor to avoid gimbal lock
	//		float limit = XM_PI / 2.0f - 0.01f;
	//		rotation.y = std::max(-limit, rotation.y);
	//		rotation.y = std::min(limit, rotation.y);

	//		// keep longitude in sane range by wrapping
	//		if (rotation.x > XM_PI)
	//		{
	//			rotation.x -= XM_PI * 2.0f;
	//		}
	//		else if (rotation.x < -XM_PI)
	//		{
	//			rotation.x += XM_PI * 2.0f;
	//		}
	//	}
	//	// keyboard
	//	Vector3 movement = Vector3::Zero;

	//	if (keyboard.Up || keyboard.W)
	//		movement.z += 1.f;

	//	if (keyboard.Down || keyboard.S)
	//		movement.z -= 1.f;

	//	if (keyboard.Left || keyboard.A)
	//		movement.x += 1.f;

	//	if (keyboard.Right || keyboard.D)
	//		movement.x -= 1.f;

	//	if (keyboard.PageUp || keyboard.Space)
	//		movement.y -= 1.f;

	//	if (keyboard.PageDown || keyboard.LeftShift)
	//		movement.y += 1.f;
	//	move(movement);


	//}

	//void Player::render() {
	//	XMFLOAT3 up, lookAt;
	//	XMVECTOR upVector, positionVector, lookAtVector;
	//	XMMATRIX rotationMatrix;

	//	// Setup the vector that points upwards.
	//	up.x = 0.0f;
	//	up.y = 1.0f;
	//	up.z = 0.0f;

	//	// Load it into a XMVECTOR structure.
	//	upVector = XMLoadFloat3(&up);

	//	// Load position into a XMVECTOR structure.
	//	positionVector = XMLoadFloat3(&position);

	//	// Setup where the camera is looking by default.
	//	lookAt.x = 0.0f;
	//	lookAt.y = 0.0f;
	//	lookAt.z = 1.0f;

	//	// Load it into a XMVECTOR structure.
	//	lookAtVector = XMLoadFloat3(&lookAt);

	//	// Create the rotation matrix from the yaw, pitch, and roll values.
	//	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.y, rotation.x, rotation.z);

	//	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	//	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	//	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	//	// Translate the rotated camera position to the location of the viewer.
	//	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	//	// Finally create the view matrix from the three updated vectors.
	//	viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
	//}

	//XMMATRIX Player::getViewMatrix() {
	//	float y = sinf(rotation.y);
	//	float r = cosf(rotation.y);
	//	float z = r*cosf(rotation.x);
	//	float x = r*sinf(rotation.x);

	//	XMVECTOR lookAt = Vector3(position) + Vector3(x, y, z);

	//	XMMATRIX view = XMMatrixLookAtRH(Vector3(position), lookAt, Vector3(0.f, 1.f, 0.f));
	//	return view;
	//}

	//void Player::renderBaseViewMatrix() {
	//	XMFLOAT3 up, lookAt;
	//	XMVECTOR upVector, positionVector, lookAtVector;
	//	XMMATRIX rotationMatrix;

	//	// Setup the vector that points upwards.
	//	up.x = 0.0f;
	//	up.y = 1.0f;
	//	up.z = 0.0f;

	//	// Load it into a XMVECTOR structure.
	//	upVector = XMLoadFloat3(&up);

	//	// Load it into a XMVECTOR structure.
	//	positionVector = XMLoadFloat3(&position);

	//	// Setup where the camera is looking by default.
	//	lookAt.x = 0.0f;
	//	lookAt.y = 0.0f;
	//	lookAt.z = 1.0f;

	//	// Load it into a XMVECTOR structure.
	//	lookAtVector = XMLoadFloat3(&lookAt);

	//	// Create the rotation matrix from the yaw, pitch, and roll values.
	//	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	//	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	//	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	//	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	//	// Translate the rotated camera position to the location of the viewer.
	//	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	//	// Finally create the view matrix from the three updated vectors.
	//	baseViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
	//}

	//void Player::getBaseViewMatrix(XMMATRIX& viewMatrix) {
	//	viewMatrix = baseViewMatrix;
	//	return;
	//}
}


