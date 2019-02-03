#pragma once
#include "WorldComponent.h"
namespace world {
	class MovementController :
		public WorldComponent
	{
	public:
		enum MovementTypes {
			Normal,
			Spectator,
			Swim
		};
		MovementController();
		// The type of movement
		MovementTypes Type;
		// The normalized direction
		Vector3 Heading;
		// Whether to jump or not
		bool Jump;
		// Speed
		float Speed;
	};
}
