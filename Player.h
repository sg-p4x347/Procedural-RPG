#pragma once
#include "Component.h"
#include <DirectXMath.h>
using namespace DirectX;
namespace Components {
	enum MovementStates {
		Normal,
		Spectator
	};
	enum InteractionStates {
		World,
		InventoryUI,
		ContainerUI,
		GamePausedUI
	};
	class Player :
		public Component {
	public:
		Player();
		// Data
		MovementStates MovementState;
		InteractionStates InteractionState;
		string InventoryCategory;
		unsigned int OpenContainer;

		// Inherited via Component
		virtual string GetName() override;

	};
}
