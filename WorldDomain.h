#pragma once
#include "WorldComponent.h"

#include "Action.h"
#include "VoxelGridModel.h"
#include "Collision.h"
#include "Inventory.h"
#include "Model.h"
#include "Movement.h"
#include "Player.h"
#include "Position.h"
#include "Global.h"
#include "Plant.h"
#include "MovementController.h"
#include "WorldEntityManager.h"
namespace world {
	typedef WorldEntityManager<
		Position, 
		Movement, 
		Player, 
		Action, 
		VoxelGridModel, 
		Collision, 
		Model,
		Global,
		MovementController,
		Plant
	> WEM;
}