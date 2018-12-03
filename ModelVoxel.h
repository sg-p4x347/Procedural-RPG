#pragma once
#include "AssetTypedefs.h"
#include "Voxel.h"
class ModelVoxel : 
	public Voxel
{
public:

	ModelVoxel();
	void AddComponent(AssetID asset, TransformID transform);
	void AddComponent(AssetID asset, Transforms transform);
	const std::vector<std::pair<AssetID, TransformID>> & GetComponents();
	// Inherited via ISerialization
	virtual void Import(std::istream & ifs) override;
	virtual void Export(std::ostream & ofs) override;
private:
	std::vector<std::pair<AssetID, TransformID>> m_components;
};

