#pragma once
#include "Component.h"
#include <DirectXMath.h>
using namespace DirectX;
namespace Components {
	enum MovementStates {
		Normal,
		Spectator
	};
	class Player :
		public Component {
	public:
		Player();
		// Data
		MovementStates MovementState;

		// Inherited via Component
		virtual string GetName() override;

	};
}
