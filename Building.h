#pragma once
#include "Room.h"
#include "BuildingVoxel.h"
#include "WorldComponent.h"
#include "ISerialization.h"
#include "Map.h"
namespace world {
class Building : 
	public WorldComponent,
	public ISerialization
{
public:
	Building();
	Building(vector<Architecture::Room> & rooms, Map<BuildingVoxel> & voxels);
	// Data
	vector<Architecture::Room> Rooms;
	Map<BuildingVoxel> Voxels;


	// Inherited via Component
	virtual void Export(std::ostream & ofs) override;
	virtual void Import(std::istream & ifs) override;
};
}