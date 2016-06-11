#include "pch.h"
#include "Entity.h"

Entity::Entity() {

}
Entity::Entity(XMFLOAT3 POSITION, float MASS) {
	position = POSITION;
	mass = MASS;
}
Entity::Entity(XMFLOAT3 POSITION, XMFLOAT3 VELOCITY, XMFLOAT3 ACCELERATION, float MASS) {
	position = POSITION;
	velocity = VELOCITY;
	acceleration = ACCELERATION;
	mass = MASS;
}
void Entity::updatePhysics(float timeStep) {
	// calculate new position
	position.x += velocity.x * timeStep;
	position.y += velocity.y * timeStep;
	position.z += velocity.z * timeStep;
	// calculate new velocity
	velocity.x += acceleration.x * timeStep;
	velocity.y += acceleration.y * timeStep;
	velocity.z += acceleration.z * timeStep;
	// calculate new rotation
	rotation.x += angularVelocity.x * XM_2PI * timeStep;
	rotation.y += angularVelocity.y * XM_2PI * timeStep;
	rotation.z += angularVelocity.z * XM_2PI * timeStep;
}
// getters
XMFLOAT3 Entity::getPosition() {
	return position;
}
XMFLOAT3 Entity::getVelocity() {
	return velocity;
}
XMFLOAT3 Entity::getAcceleration(){
	return acceleration;
}
float Entity::getMass() {
	return mass;
}
// setters
void Entity::setPosition(XMFLOAT3 pos) {
	position = pos;
}
void Entity::setAcceleration(XMFLOAT3 acc) {
	acceleration = acc;
}
void Entity::setRotation(XMFLOAT3 rot) {
	rotation = rot;
}
Entity::~Entity() {

}
