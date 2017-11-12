#include "pch.h"
#include "Occupation.h"


Occupation::Occupation(OccupationType t) {
	type = t;
	population = 10;
	// universal demands
	inventory.items["food"].demand = 1.f;
	inventory.items["water"].demand = 1.f;
	inventory.items["tools"].demand = 1.f;
	// specific supply and demands
	switch (type) {
	case Logger:
		inventory.items["wood"].supply = 2.f;
		break;
	case Miner:
		inventory.items["stone"].supply = 1.f;
		inventory.items["iron"].supply = 2.f;
		inventory.items["coal"].supply = 2.f;
		inventory.items["gold"].supply = 1.f;
		inventory.items["silver"].supply = 1.f;
		break;
	case Blacksmith:
		inventory.items["tools"].supply = 10.f;
		inventory.items["iron"].demand = 1.f;
		inventory.items["coal"].demand = 1.f;
		break;
	case Soldier:
		inventory.items["safety"].supply = 10.f;
		break;
	case Farmer:
		inventory.items["food"].supply = 2.f;
		inventory.items["water"].supply = 2.f;
		break;
	case Carpenter:
		inventory.items["shelter"].supply = 2.f;
		inventory.items["wood"].demand = 1.f;
		inventory.items["stone"].demand = 1.f;
		break;
	}
}
void Occupation::Update()
{
	// determine the carrying capacity of this occupation
	int capacity;
	capacity = inventory.items["shelter"].quantity * 4; // 4 per household
	int difference = capacity - population;
	// move half way to the capacity
	population += difference / 2;
}
