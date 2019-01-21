#pragma once
#include "PersistenceEntityManager.h"
#include "ModelAsset.h"
#include "CollisionAsset.h"
#include "PathID.h"
#include "VboParser.h"
#include "VBO.h"
#include "Item.h"
#include "HeightMapAsset.h"
#include "NormalMapAsset.h"

class AssetEntityManager :
	public PersistenceEntityManager
{
public:
	AssetEntityManager(Filesystem::path directory);
	//----------------------------------------------------------------
	// Queries
	shared_ptr<ModelAsset> GetModel(string path);
	bool TryFindByPathID(string path, EntityPtr & entity);
	
	//----------------------------------------------------------------
	// Factories
	EntityPtr CreateAsset(string path) {
		EntityPtr entity = NewEntity();
		entity->AddComponent(new PathID(path));
		return entity;
	}
	virtual void CollectGarbage() override;
private:
	
	// Asset lookup
	map<string, EntityPtr> m_assets;
	
	// Utility
	VboParser m_vboParser;
};

