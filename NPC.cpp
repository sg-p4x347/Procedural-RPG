#include "NPC.h"

NPC::NPC() {
}
NPC::NPC(XMFLOAT3 POSITION, float MASS) : Entity( POSITION, MASS) {
}
NPC::~NPC() {
}
// protected
void NPC::moveForward() {
	// forward
	velocity.x = (sin(rotation.x) + sin(rotation.z))*cos(rotation.y)*movementSpeed;
	velocity.y = (sin(-rotation.y))*movementSpeed;
	velocity.z = (sin(rotation.z) + cos(rotation.x))*cos(rotation.y)*movementSpeed;
}
void NPC::moveBackward() {
	// backward
	velocity.x = -(sin(rotation.x) + sin(rotation.z))*cos(rotation.y)*movementSpeed;
	velocity.y = -(sin(-rotation.y))*movementSpeed;
	velocity.z = -(sin(rotation.z) + cos(rotation.x))*cos(rotation.y)*movementSpeed;
}
void NPC::stop() {
	velocity = XMFLOAT3(0, 0, 0);
}