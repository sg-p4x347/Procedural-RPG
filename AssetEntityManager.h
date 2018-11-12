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
	template <typename VertexType>
	EntityPtr CreateModel(string path, vector<shared_ptr<Components::VBO<VertexType>>> vbos, int lodSpacing,string effect) {
		EntityPtr entity = NewEntity();
		entity->AddComponent(new ModelAsset(lodSpacing,vbos.size(), effect));
		entity->AddComponent(new PathID(path));
		for (int lod = 0; lod < vbos.size(); lod++) {
			m_vboParser.ExportVBO<VertexType>(path + '_' + to_string(lod), *(vbos[lod]));
		}
		return entity;
	}
	virtual void CollectGarbage() override;
private:
	
	// Asset lookup
	map<string, EntityPtr> m_assets;
	
	// Utility
	VboParser m_vboParser;
};

