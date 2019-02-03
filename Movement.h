#pragma once
#include "WorldComponent.h"

namespace world {
	class Movement :
		public WorldComponent
	{
	public:
		Movement(
			DirectX::SimpleMath::Vector3 velocity = Vector3::Zero,
			DirectX::SimpleMath::Vector3 acceleration = Vector3(0,-9.8,0),
			DirectX::SimpleMath::Vector3 angularVelocity = Vector3::Zero,
			DirectX::SimpleMath::Vector3 angularAcceleration = Vector3::Zero);

		DirectX::SimpleMath::Vector3 Velocity;
		DirectX::SimpleMath::Vector3 Acceleration;
		DirectX::SimpleMath::Vector3 AngularVelocity;
		DirectX::SimpleMath::Vector3 AngularAcceleration;

		// Inherited via Component

	};
}

