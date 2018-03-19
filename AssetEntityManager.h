#pragma once
#include "PersistenceEntityManager.h"
#include "ModelAsset.h"
#include "PathID.h"
#include "VboParser.h"
#include "VBO.h"
#include "Item.h"
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
	EntityPtr CreateModel(string path, Components::VBO<VertexType> & vbo,int lod = 0) {
		EntityPtr entity = NewEntity();
		entity->AddComponent(new ModelAsset());
		entity->AddComponent(new PathID(path));
		m_vboParser.ExportVBO<VertexType>(path + '_' + to_string(lod), vbo);
		return entity;
	}
private:
	
	// Asset lookup
	map<string, EntityPtr> m_assets;
	
	// Utility
	VboParser m_vboParser;
};

