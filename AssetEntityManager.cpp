#include "pch.h"
#include "AssetEntityManager.h"

AssetEntityManager::AssetEntityManager(Filesystem::path directory) : PersistenceEntityManager::PersistenceEntityManager(directory), m_vboParser(directory)
{
	RegisterComponent([] {return new ModelAsset();});
	RegisterComponent([] {return new PathID();});
	RegisterComponent([] {return new Item();});
}

shared_ptr<ModelAsset> AssetEntityManager::GetModel(string path)
{
	//if (m_models.find(path) != m_models.end()) {
	//	return m_models[path];
	//}
	//shared_ptr<ModelAsset> result;
	//for (auto entity : FindEntities("ModelAsset")) {
	//	auto modelAsset = entity->GetComponent<ModelAsset>("ModelAsset");
	//	if (modelAsset->Path == path) result = modelAsset; // result found
	//	// Cache this component in the map
	//	if (m_models.find(modelAsset->Path) == m_models.end()) {
	//		m_models.insert(std::make_pair(modelAsset->Path, modelAsset));
	//	}
	//}
	EntityPtr entity;
	if (TryFindByPathID(path, entity)) {
		return entity->GetComponent<ModelAsset>("ModelAsset");
	}
	return nullptr;
}

bool AssetEntityManager::TryFindByPathID(string path, EntityPtr & entity)
{
	if(m_assets.find(path) != m_assets.end()) {
		entity = m_assets[path];
		return true;
	}
	for (auto e : FindEntities("PathID")) {
		auto pathID = e->GetComponent<PathID>("PathID");
		if (pathID->Path == path) {
			// result found
			entity = e;
			// Cache this component in the map
			m_assets.insert(std::make_pair(path, entity));
			return true;
		}
	}
	return false;
}
