#include "pch.h"
#include "NPC.h"

using namespace DirectX::SimpleMath;

NPC::NPC() {
}
NPC::NPC(XMFLOAT3 POSITION, float MASS) : Entity( POSITION, MASS) {
}
NPC::~NPC() {
}
// protected
void NPC::move(Vector3 movement) {

	Quaternion q = Quaternion::CreateFromYawPitchRoll(rotation.x, rotation.y , 0.f);
	movement = Vector3::Transform(movement, q);

	movement *= 1.5;
	position.x += movement.x;
	position.y -= movement.y;
	position.z += movement.z;
}
void NPC::stop() {
	velocity = XMFLOAT3(0, 0, 0);
}