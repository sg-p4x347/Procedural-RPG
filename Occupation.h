#pragma once

#include "Inventory.h"

enum OccupationType {
	Logger,
	Miner,
	Blacksmith,
	Soldier,
	Farmer,
	Carpenter
};
struct Occupation {
	Occupation(OccupationType t);
	void Update();
	OccupationType type;
	int population;
	Inventory inventory;
};

