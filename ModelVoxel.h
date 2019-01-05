#pragma once
#include "AssetTypedefs.h"
#include "Voxel.h"
#include "CollisionModel.h"
class ModelVoxel : 
	public Voxel
{
public:

	ModelVoxel();
	void AddComponent(AssetID asset, TransformID transform);
	void AddComponent(AssetID asset, Transforms transform);
	void Clear();
	const std::set<std::pair<AssetID, TransformID>> & GetComponents() const;
	shared_ptr<geometry::CollisionModel> GetCollision();
	void SetCollision(shared_ptr<geometry::CollisionModel> & collision);
	// Inherited via ISerialization
	virtual void Import(std::istream & ifs) override;
	virtual void Export(std::ostream & ofs) override;
private:
	std::set<std::pair<AssetID, TransformID>> m_components;
	// Collision cache
	shared_ptr<geometry::CollisionModel> m_collision;
};

