#pragma once
#include "WorldComponent.h"
#include "ISerialization.h"
#include "VoxelGrid.h"
#include "ModelVoxel.h"
namespace world {
class VoxelGridModel : 
	public WorldComponent,
	public ISerialization
{
public:
	VoxelGridModel();
	VoxelGridModel(VoxelGrid<ModelVoxel> & voxels);
	// Data
	VoxelGrid<ModelVoxel> Voxels;

	// Inherited via Component
	virtual void Export(std::ostream & ofs) override;
	virtual void Import(std::istream & ifs) override;
};
}