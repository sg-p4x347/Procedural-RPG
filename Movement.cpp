#include "pch.h"
#include "Movement.h"

namespace world {
	Movement::Movement() :
		Movement::Movement(
			DirectX::SimpleMath::Vector3::Zero,
			DirectX::SimpleMath::Vector3::Zero,
			DirectX::SimpleMath::Vector3::Zero,
			DirectX::SimpleMath::Vector3::Zero
		)
	{
	}

	Movement::Movement(
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

}
