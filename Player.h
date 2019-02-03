#pragma once
#include "WorldComponent.h"

namespace world {
	
	class Player :
		public WorldComponent {
	public:
		enum InteractionStates {
			FirstPerson,
			ThirdPerson,
			InventoryUI,
			ContainerUI,
			GamePausedUI
		};
		Player();
		// Data
		InteractionStates InteractionState;
		float CameraPitch;
		//string InventoryCategory;
		unsigned int OpenContainer;

	};
}
