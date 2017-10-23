#pragma once
#include "Component.h"
#include "Movement.h"

namespace Component {
	class Movement :
		public Component
	{
	public:
		Movement();
		DirectX::SimpleMath::Vector3 Velocity;
		DirectX::SimpleMath::Vector3 Acceleration;
		DirectX::SimpleMath::Vector3 AngularVelocity;
		DirectX::SimpleMath::Vector3 AngularAcceleration;

		// Inherited via Component
		
	};
}

