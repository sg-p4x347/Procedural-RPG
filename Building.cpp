#include "pch.h"

#include "Line.h"
#include "JsonParser.h"
#include "Utility.h"
#include "Room.h"
#include "Building.h"
#include "Rectangle.h"
#include "BuildingVoxel.h"

using namespace Architecture;

Components::Building::Building()
{
}

Components::Building::Building(vector<Room>& rooms, Map<BuildingVoxel>& voxels) : Rooms(rooms), Voxels(voxels)
{
}

string Components::Building::GetName()
{
	return "Building";
}

void Components::Building::Export(ofstream & ofs)
{
	Voxels.Export(ofs);
	for (int x = 0; x <= Voxels.width; x++) {
		for (int z = 0; z <= Voxels.length; z++) {
			Voxels.map[x][z].Export(ofs);
		}
	}
}

void Components::Building::Import(ifstream & ifs)
{
	Voxels.Import(ifs);
	for (int x = 0; x <= Voxels.width; x++) {
		for (int z = 0; z <= Voxels.length; z++) {
			Voxels.map[x][z].Import(ifs);
		}
	}
}
