#include "pch.h"
#include "Movement.h"


Components::Movement::Movement() : 
	Movement::Movement(
		DirectX::SimpleMath::Vector3::Zero,
		DirectX::SimpleMath::Vector3::Zero,
		DirectX::SimpleMath::Vector3::Zero,
		DirectX::SimpleMath::Vector3::Zero
	)
{
}

Components::Movement::Movement(
	DirectX::SimpleMath::Vector3 velocity,
	DirectX::SimpleMath::Vector3 acceleration,
	DirectX::SimpleMath::Vector3 angularVelocity,
	DirectX::SimpleMath::Vector3 angularAcceleration) :
	Velocity(velocity),
	Acceleration(acceleration),
	AngularVelocity(angularVelocity),
	AngularAcceleration(angularAcceleration)
{
}

string Components::Movement::GetName()
{
	return "Movement";
}

