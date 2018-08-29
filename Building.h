#pragma once
#include "Room.h"
#include "BuildingVoxel.h"
#include "Component.h"
#include "Map.h"
	namespace Components {
class Building : 
	public Component
{
public:
	Building();
	Building(vector<Architecture::Room> & rooms, Map<BuildingVoxel> & voxels);
	// Data
	vector<Architecture::Room> Rooms;
	Map<BuildingVoxel> Voxels;


	// Inherited via Component
	virtual string GetName() override;
	virtual void Export(std::ostream & ofs) override;
	virtual void Import(std::istream & ifs) override;
};
}