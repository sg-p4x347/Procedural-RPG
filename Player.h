#pragma once
#include "WorldComponent.h"

namespace world {
	
	class Player :
		public WorldComponent {
	public:
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
		Player();
		// Data
		MovementStates MovementState;
		InteractionStates InteractionState;
		float CameraPitch;
		//string InventoryCategory;
		unsigned int OpenContainer;

	};
}
