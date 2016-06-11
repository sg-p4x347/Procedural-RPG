#include "pch.h"
#include "Player.h"

Player::Player() {

}

Player::Player(XMFLOAT3 POSITION, float MASS) : NPC(POSITION,MASS) {

}

Player::~Player() {
}
void Player::update() {
	// set velocity based on user input
	
	//// forward
	//float vel = 50;
	//if (input->IsForwardPressed() && !input->IsBackPressed()) {
	//	moveForward();
	//} else if (input->IsBackPressed() && !input->IsForwardPressed()) {
	//	moveBackward();
	//} else {
	//	stop();
	//}
	//// Up
	//if (input->IsSpacePressed()) {
	//	velocity.y += vel;
	//}
	//
	//// Pitch
	//if (input->IsUpArrowPressed()) {
	//	angularVelocity.y = -0.5;
	//} else if (input->IsDownArrowPressed()) {
	//	angularVelocity.y = 0.5;
	//} else {
	//	angularVelocity.y = 0;
	//}
	//// yaw 
	//if (input->IsLeftArrowPressed()) {
	//	angularVelocity.x = -0.5;
	//} else if (input->IsRightArrowPressed()) {
	//	angularVelocity.x = 0.5;
	//} else {
	//	angularVelocity.x = 0;
	//}
}
void Player::render() {
	XMFLOAT3 up, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	XMMATRIX rotationMatrix;

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Load it into a XMVECTOR structure.
	upVector = XMLoadFloat3(&up);

	// Load position into a XMVECTOR structure.
	positionVector = XMLoadFloat3(&position);

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Load it into a XMVECTOR structure.
	lookAtVector = XMLoadFloat3(&lookAt);

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.y, rotation.x, rotation.z);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Finally create the view matrix from the three updated vectors.
	viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void Player::getViewMatrix(XMMATRIX& returnViewMatrix) {
	returnViewMatrix = viewMatrix;
	return;
}

void Player::renderBaseViewMatrix() {
	XMFLOAT3 up, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	XMMATRIX rotationMatrix;

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Load it into a XMVECTOR structure.
	upVector = XMLoadFloat3(&up);

	// Load it into a XMVECTOR structure.
	positionVector = XMLoadFloat3(&position);

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Load it into a XMVECTOR structure.
	lookAtVector = XMLoadFloat3(&lookAt);

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Finally create the view matrix from the three updated vectors.
	baseViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void Player::getBaseViewMatrix(XMMATRIX& viewMatrix) {
	viewMatrix = baseViewMatrix;
	return;
}