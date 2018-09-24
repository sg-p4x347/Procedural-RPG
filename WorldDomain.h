#pragma once
#include "WorldComponent.h"

#include "Action.h"
#include "Building.h"
#include "Collision.h"
#include "Inventory.h"
#include "Model.h"
#include "Movement.h"
#include "Player.h"
#include "Position.h"
#include "Terrain.h"
#include "Water.h"

#include "WorldEntityManager.h"
namespace world {
	typedef WorldEntityManager<Position, Movement, Player, Action, Building, Collision, Inventory, Model, Terrain,Water> WEM;
}