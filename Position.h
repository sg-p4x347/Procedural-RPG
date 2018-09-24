#pragma once
#include "Position.h"
#include "WorldComponent.h"



namespace world {
	class Position : public WorldComponent
	{
	public:
		Position(SimpleMath::Vector3 pos = SimpleMath::Vector3::Zero, SimpleMath::Vector3 rot = SimpleMath::Vector3::Zero);
		// Position (x,y,z)
		SimpleMath::Vector3 Pos;
		// Rotation (x,y,z)
		SimpleMath::Vector3 Rot;
		
		// Inherited via Component
	};
}

