#include "pch.h"
#include "AssetEntityManager.h"
#include "ModelAsset.h"
AssetEntityManager::AssetEntityManager(Filesystem::path directory) : PersistenceEntityManager::PersistenceEntityManager(directory), m_vboParser(directory)
{
	RegisterComponent([] {return new ModelAsset();});
}

shared_ptr<ModelAsset> AssetEntityManager::GetModel(string path)
{
	if (m_models.find(path) != m_models.end()) {
		return m_models[path];
	}
	shared_ptr<ModelAsset> result;
	for (auto entity : FindEntities("ModelAsset")) {
		auto modelAsset = entity->GetComponent<ModelAsset>("ModelAsset");
		if (modelAsset->Path == path) result = modelAsset; // result found
		// Cache this component in the map
		if (m_models.find(modelAsset->Path) == m_models.end()) {
			m_models.insert(std::make_pair(modelAsset->Path, modelAsset));
		}
	}
	return result;
}
