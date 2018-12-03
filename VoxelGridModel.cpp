#include "pch.h"
#include "VoxelGridModel.h"
#include "JsonParser.h"

namespace world {
	VoxelGridModel::VoxelGridModel()
	{
	}

	VoxelGridModel::VoxelGridModel(VoxelGrid<ModelVoxel> & voxels) : Voxels(voxels)
	{
	}

	void VoxelGridModel::Export(std::ostream & ofs)
	{
		Voxels.Export(ofs);
	}

	void VoxelGridModel::Import(std::istream & ifs)
	{
		Voxels.Import(ifs);
	}
}
