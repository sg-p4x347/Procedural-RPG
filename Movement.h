#pragma once
#include "Component.h"
#include "Movement.h"

namespace Components {
	class Movement :
		public Component
	{
	public:
		Movement(const unsigned int & id);
		Movement(const unsigned int & id,
			DirectX::SimpleMath::Vector3 velocity,
			DirectX::SimpleMath::Vector3 acceleration,
			DirectX::SimpleMath::Vector3 angularVelocity,
			DirectX::SimpleMath::Vector3 angularAcceleration);
		~Movement();
		DirectX::SimpleMath::Vector3 Velocity;
		DirectX::SimpleMath::Vector3 Acceleration;
		DirectX::SimpleMath::Vector3 AngularVelocity;
		DirectX::SimpleMath::Vector3 AngularAcceleration;

		// Inherited via Component
		virtual string GetName() override;

	};
}

