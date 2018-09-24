#include "pch.h"

#include "Line.h"
#include "JsonParser.h"
#include "Utility.h"
#include "Room.h"
#include "Building.h"
#include "Rectangle.h"
#include "BuildingVoxel.h"

using namespace Architecture;
namespace world {
	Building::Building()
	{
	}

	Building::Building(vector<Room>& rooms, Map<BuildingVoxel>& voxels) : Rooms(rooms), Voxels(voxels)
	{
	}

	void Building::Export(std::ostream & ofs)
	{
		Voxels.Export(ofs);
		for (int x = 0; x <= Voxels.width; x++) {
			for (int z = 0; z <= Voxels.length; z++) {
				Voxels.map[x][z].Export(ofs);
			}
		}
	}

	void Building::Import(std::istream & ifs)
	{
		Voxels.Import(ifs);
		for (int x = 0; x <= Voxels.width; x++) {
			for (int z = 0; z <= Voxels.length; z++) {
				Voxels.map[x][z].Import(ifs);
			}
		}
	}
}
