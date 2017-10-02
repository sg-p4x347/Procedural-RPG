#pragma once
#include "Component.h"
#include "Movement.h"

namespace {
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
		virtual shared_ptr<Component> GetComponent(const unsigned int & id) override;
		virtual void Attach(const unsigned int & id) override;
		virtual void Save(string directory) override;
		virtual void Load(string directory) override;
	};
}

