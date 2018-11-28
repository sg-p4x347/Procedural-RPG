#pragma once
#include "WorldComponent.h"
#include "ISerialization.h"
#include "VoxelGrid.h"
#include "BuildingVoxel.h"
namespace world {
class VoxelGridModel : 
	public WorldComponent,
	public ISerialization
{
public:
	VoxelGridModel();
	VoxelGridModel(VoxelGrid<BuildingVoxel> & voxels);
	// Data
	VoxelGrid<BuildingVoxel> Voxels;

	// Inherited via Component
	virtual void Export(std::ostream & ofs) override;
	virtual void Import(std::istream & ifs) override;
};
}