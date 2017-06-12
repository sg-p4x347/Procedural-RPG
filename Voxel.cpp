#include "pch.h"
#include "Utility.h"
#include "Voxel.h"

using namespace DirectX;
using namespace Utility;
Voxel::Voxel()
{
	InitArray<short>(m_floors, 4, 0);
	InitArray<short>(m_walls, 8, 0);
	InitArray<short>(m_corners, 4, 0);
}

Voxel::Voxel(JsonParser & voxel)
{
	Import(voxel);
}

void Voxel::Wall(short unitX, short unitY, short interiorType)
{
	if (abs(unitX) == 1) {
		// vertical walls
		if (unitY >= 0) {
			// -y wall
			SetWall(3, sign(unitX), interiorType);
		}
		if (unitY <= 0) {
			// +y wall
			SetWall(1, sign(unitX), interiorType);
		}
	}
	if (abs(unitY) == 1) {
		// horizontal walls
		if (unitX >= 0) {
			// -x wall
			SetWall(2, sign(unitY), interiorType);
		}
		if (unitX <= 0) {
			// +x wall
			SetWall(0, sign(unitY), interiorType);
		}
	}
}

void Voxel::Floor(short unitX, short unitY, short floorType)
{
	if (unitX >= 0) {
		// -x floors
		if (unitY >= 0) {
			// -y floor
			SetFloor(2, floorType);
		}
		if (unitY <= 0) {
			// +y floor
			SetFloor(1, floorType);
		}
	}
	if (unitX <= 0) {
		// +x floors
		if (unitX >= 0) {
			// -y floor
			SetFloor(3, floorType);
		}
		if (unitX <= 0) {
			// +y floor
			SetFloor(2, floorType);
		}
	}
}

void Voxel::SetWall(short cardinalIndex, short exteriorSign, short interiorType)
{
	// cardinalIndex {0 = +x, 1 = +y, 2 = -x, 3 = -y}
	int interiorIndex = 0;
	int exteriorIndex = 1;
	if (exteriorSign < 0) {
		// exterior is in the positive x or y direction
		exteriorIndex = cardinalIndex;
		interiorIndex = cardinalIndex + 1;
	} else {
		// exterior is in the negative x or y direction
		interiorIndex = cardinalIndex;
		exteriorIndex = cardinalIndex + 1;
	}
	m_walls[interiorIndex] = interiorType;
}

void Voxel::SetFloor(short cornerIndex, short floorType)
{
	// cornerIndex: {0 = +x+y, 1 = -x+y, 2 = -x-y, 3 = +x-y}
	if (cornerIndex < 4) {
		m_floors[cornerIndex] == floorType;
	}
}

void Voxel::Import(JsonParser & voxel)
{
	for (int i = 0; i < voxel["floors"].Count(); i++) {
		m_floors[i] = voxel["floors"][i].To<short>();
	}
	for (int i = 0; i < voxel["walls"].Count(); i++) {
		m_walls[i] = voxel["walls"][i].To<short>();
	}
	for (int i = 0; i < voxel["corners"].Count(); i++) {
		m_corners[i] = voxel["corners"][i].To<short>();
	}
}

JsonParser Voxel::Export()
{
	JsonParser voxel;
	JsonParser floors;
	JsonParser walls;
	JsonParser corners;
	for (int i = 0; i < 8; i++) {
		if (i < 4) {
			floors.Add(m_floors[i]);
			corners.Add(m_corners[i]);
		}
		walls.Add(m_walls[i]);
	}
	voxel.Set("floors", floors);
	voxel.Set("walls", walls);
	voxel.Set("corners", corners);
	return voxel;
}
