#pragma once
#include "PersistenceEntityManager.h"
#include "ModelAsset.h"
#include "VboParser.h"
#include "VBO.h"
class AssetEntityManager :
	public PersistenceEntityManager
{
public:
	AssetEntityManager(Filesystem::path directory);
	//----------------------------------------------------------------
	// Accessors
	shared_ptr<ModelAsset> GetModel(string path);
	//----------------------------------------------------------------
	// Factories
	template <typename VertexType>
	EntityPtr CreateModel(string path, Components::VBO<VertexType> & vbo,int lod = 0) {
		EntityPtr entity = NewEntity();
		entity->AddComponent(new ModelAsset(path));
		m_vboParser.ExportVBO<VertexType>(path + '_' + to_string(lod), vbo);
		return entity;
	}
private:
	map<string, shared_ptr<ModelAsset>> m_models;
	VboParser m_vboParser;
};

