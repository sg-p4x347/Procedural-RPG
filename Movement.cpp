#include "pch.h"
#include "Movement.h"


Components::Movement::Movement(const unsigned int & id) : 
	Movement::Movement(id,
		DirectX::SimpleMath::Vector3::Zero,
		DirectX::SimpleMath::Vector3::Zero,
		DirectX::SimpleMath::Vector3::Zero,
		DirectX::SimpleMath::Vector3::Zero
	)
{
}

Components::Movement::Movement(const unsigned int & id,
	DirectX::SimpleMath::Vector3 velocity,
	DirectX::SimpleMath::Vector3 acceleration,
	DirectX::SimpleMath::Vector3 angularVelocity,
	DirectX::SimpleMath::Vector3 angularAcceleration) :
	Components::Component(id),
	Velocity(velocity),
	Acceleration(acceleration),
	AngularVelocity(angularVelocity),
	AngularAcceleration(angularAcceleration)
{
}

Components::Movement::~Movement()
{
	Save(Component::GetDirectory());
}

string Components::Movement::GetName()
{
	return "Movement";
}

