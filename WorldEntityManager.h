#pragma once
#include "Position.h"
#include "Player.h"
#include "Region.h"
#include "WorldEntityCache.h"
#include "EntityIndex.h"
#include "WorldEntityProxy.h"
#include "IEventManager.h"
namespace world {
	template<typename ... CompTypes>
	class WorldEntityManager
	{
		
	public:
		typedef Region<CompTypes...> RegionType;
		WorldEntityManager(Filesystem::path directory, unsigned int worldWidth, unsigned int regionWidth, float loadRange) :
			m_worldWidth(worldWidth),
			m_regionWidth(regionWidth),
			m_regionDimension(worldWidth / regionWidth),
			m_entityIndex(directory),
			m_player(1),
			m_loadRange(loadRange),
			m_playerCache(NewEntityCache<Position,Player>())
		{
			SetMask<CompTypes...>();

			m_regions = vector<vector<shared_ptr<Region<CompTypes...>>>>(m_regionDimension, vector<shared_ptr<Region<CompTypes...>>>(m_regionDimension));

			for (unsigned int x = 0; x < m_regionDimension; x++) {
				for (unsigned int z = 0; z < m_regionDimension; z++) {
					m_regions[x][z] = make_shared<Region<CompTypes...>>(directory / (std::to_string(x) + ',' + std::to_string(z)), m_maskIndex,DirectX::SimpleMath::Rectangle(x * m_regionWidth,z * m_regionWidth,m_regionWidth,m_regionWidth));
					//m_loadedRegions.insert(m_regions[x][z]);
				}
			}

			RegisterHandlers();

		}
		~WorldEntityManager() {
			Shutdown();
			Save();
		}

		void RegisterHandlers() {
			// Re-center the loaded regions on the player position
			IEventManager::RegisterHandler(EventTypes::Movement_PlayerMoved, std::function<void(float, float)>([=](float x, float z) {
				ReCenter(x, z, m_loadRange);
			}));
		}
		EntityID PlayerID() {
			return m_player;
		}
		Vector3 PlayerPos() {
			return GetEntity<Position>(m_player)->Get<Position>().Pos;
		}
		MaskType PlayerSignature() {
			return GetSignature(m_player);
		}
		MaskType GetSignature(EntityID & id) {
			EntityInfo * info;
			m_entityIndex.Find(id, info);
			return info->signature;
		}
		template <typename HeadType>
		MaskType GetMask() {
			return std::get<pair<MaskType, HeadType>>(m_maskIndex).first;
		}

		template<typename HeadType, typename Next, typename ... MaskTypes>
		MaskType GetMask() {
			return GetMask<HeadType>() | GetMask<Next, MaskTypes...>();
		}

		unsigned int GetRegionWidth() {
			return m_regionWidth;
		}
		const unsigned int GetWorldWidth() {
			return m_worldWidth;
		}
		template<typename ... SigTypes>
		EntityID CreateEntity(Position && position, SigTypes && ... components) {
			MaskType signature = GetMask<Position, SigTypes...>();
			int regionX;
			int regionZ;
			RegionCoords(position.Pos, regionX, regionZ);
			auto & region = m_regions[regionX][regionZ];
			EntityID id = m_entityIndex.New(EntityInfo(signature, regionX, regionZ));
			InsertComponent(*region, id, signature, position, components...);
			IEventManager::Invoke(EventTypes::Entity_SignatureChanged, region, id, signature);
			return id;
		}
		template<typename ... SigTypes>
		EntityID CreatePlayer(Position && position, SigTypes && ... components) {
			m_player = CreateEntity(std::move(position), std::move(components)...);
			return m_player;
		}

		template<typename CompType>
		void AddComponent(EntityID id, CompType && component) {

			EntityInfo * info;
			if (m_entityIndex.Find(id, info)) {
				auto & region = *m_regions[info->regionX][info->regionZ];
				ChangeSignature(id, info->signature | GetMask<CompType>());
				InsertComponent(region, id, info->signature, component);
			}

		}
		void ChangeSignature(EntityID id, MaskType signature) {
			EntityInfo * info;
			if (m_entityIndex.Find(id, info)) {
				auto region = m_regions[info->regionX][info->regionZ];
				if (region) {
					region->ChangeSignature(id, info->signature, std::move(signature));
					info->signature = signature;
					if (!signature) {
						// entity is deleted
						m_entityIndex.Delete(id);
					}
					IEventManager::Invoke(EventTypes::Entity_SignatureChanged, region, id, signature);
				}
			}
		}
		void UpdatePosition(EntityID id, Vector3 position) {
			// get info
			EntityInfo * info;
			if (m_entityIndex.Find(id, info)) {
				int regionX;
				int regionZ;
				RegionCoords(position, regionX, regionZ);
				if (regionX != info->regionX || regionZ != info->regionZ) {
					// entity has changed regions
					auto & oldRegion = m_regions[info->regionX][info->regionZ];
					auto & newRegion = m_regions[regionX][regionZ];
					EntityCache<CompTypes...> oldCache;
					oldRegion->LoadEntities<CompTypes...>(oldCache, [=](MaskType & sig) {
						return sig == info->signature;
					});
					// move components into the new region
					newRegion->ChangeSignature<tuple<shared_ptr<ComponentCache<CompTypes>>...>,CompTypes...>(oldCache.CachesFor(info->signature), id, info->signature, info->signature);
					// update the index
					info->regionX = regionX;
					info->regionZ = regionZ;
					// sync regions if this is the player
					if (id == m_player) {
						IEventManager::Invoke(EventTypes::Movement_PlayerMoved, position.x, position.z);
					}
					// Signature changed to 0 (removed) for old region
					IEventManager::Invoke(EventTypes::Entity_SignatureChanged, oldRegion, id, 0);
					// Signature changed to current (added) for new region
					IEventManager::Invoke(EventTypes::Entity_SignatureChanged, newRegion, id, info->signature);
				}
				
			}
		}
		/*template<typename CompType>
		vector<shared_ptr<ComponentCache<CompType>>> LoadComponents(MaskType signature) {
			vector<shared_ptr<ComponentCache<CompType>>> caches;
			for (auto & region : m_loadedRegions) {
				for (auto & cache : region->LoadComponents<CompType>(signature)) {
					caches.push_back(cache);
				}
			}
			return caches;
		}*/
		template<typename HeadType, typename ... MaskTypes>
		WorldEntityCache<Region<CompTypes...>, HeadType, MaskTypes...> NewEntityCache() {
			return WorldEntityCache<Region<CompTypes...>, HeadType, MaskTypes...>(GetMask<HeadType, MaskTypes...>());
		}
		// Cherry pick a set of components by entity ID
		template<typename HeadType, typename ... MaskTypes>
		shared_ptr<WorldEntityProxy<HeadType, MaskTypes...>> GetEntity(EntityID id) {
			std::lock_guard<std::recursive_mutex> lock(m_mutex);
			EntityInfo * info;
			if (m_entityIndex.Find(id, info)) {
				auto & region = *m_regions[info->regionX][info->regionZ];
				EntityCache<HeadType, MaskTypes...> entityCache;
				region.LoadEntities(entityCache, [&](world::MaskType & signature) {
					return signature == info->signature;
				});
				for (auto & entity : entityCache) {
					if (entity.GetID() == id)
						return std::make_shared<WorldEntityProxy<HeadType, MaskTypes...>>(entity.GetProxy());
				}
			}
			return nullptr;
		}
		
		template<typename HeadType, typename ... MaskTypes>
		void UpdateCache(WorldEntityCache<Region<CompTypes...>, HeadType, MaskTypes...> & cache, world::MaskType componentMask = 0, bool exclusive = false) {
			componentMask |= cache.GetComponentMask();
			UpdateCache(cache, [&](world::MaskType & signature) {
				return !exclusive && (signature & componentMask) == componentMask || exclusive && signature == componentMask;
			});
		}
		template<typename HeadType, typename ... MaskTypes>
		void UpdateCache(WorldEntityCache<Region<CompTypes...>, HeadType, MaskTypes...> & cache, std::function<bool(world::MaskType &)> && predicate) {
			std::lock_guard<std::recursive_mutex> lock(m_mutex);
			// iterate over the cached regions
			unordered_set<shared_ptr<Region<CompTypes...>>> staleRegions;
			for (auto & regionCache : cache.GetCaches()) {
				if (m_loadedRegions.count(regionCache.first) == 0) {
					staleRegions.insert(regionCache.first);
				}
			}
			// remove stale region caches
			for (auto & staleRegion : staleRegions) {
				cache.GetCaches().erase(staleRegion);
			}
			// add new region caches to cache
			for (auto & loadedRegion : m_loadedRegions) {
				if (!cache.GetCaches().count(loadedRegion)) {
					EntityCache<HeadType, MaskTypes...> entityCache;
					loadedRegion->LoadEntities<HeadType, MaskTypes...>(entityCache, std::move(predicate));
					cache.GetCaches().insert(make_pair(loadedRegion, entityCache));
					//cache.GetCaches().insert(make_pair(loadedRegion, LoadEntities<HeadType, MaskTypes...>(cache.GetComponentMask())));
				}
				else {
					// update existing caches
					loadedRegion->LoadEntities<HeadType,MaskTypes...>(cache.GetCaches()[loadedRegion], std::move(predicate));
				}
			}
		}

		// Not restricted by the loaded region subset
		template<typename HeadType, typename ... MaskTypes>
		void UpdateGlobalCache(WorldEntityCache<Region<CompTypes...>, HeadType, MaskTypes...> & cache, world::MaskType componentMask = 0, bool exclusive = false) {
			componentMask |= cache.GetComponentMask();
			UpdateGlobalCache(cache, [&](world::MaskType & signature) {
				return !exclusive && (signature & componentMask) == componentMask || exclusive && signature == componentMask;
			});
		}
		// Not restricted by the loaded region subset
		template<typename HeadType, typename ... MaskTypes>
		void UpdateGlobalCache(WorldEntityCache<Region<CompTypes...>, HeadType, MaskTypes...> & cache,std::function<bool(world::MaskType &)> && predicate) {
			std::lock_guard<std::recursive_mutex> lock(m_mutex);
			for (auto & regionRow : m_regions) {
				for (auto & region : regionRow) {
					if (!cache.GetCaches().count(region)) {
						EntityCache<HeadType, MaskTypes...> entityCache;
						region->LoadEntities<HeadType, MaskTypes...>(entityCache, std::move(predicate));
						cache.GetCaches().insert(make_pair(region, entityCache));
					}
				}
			}
		}

		RegionType & GetRegion(int x, int z) {
			return *(m_regions[x][z]);
		}
		std::set<shared_ptr<RegionType>> GetRegions() {
			std::set<shared_ptr<RegionType>> regions;
			for (auto & regionRow : m_regions) {
				for (auto & region : regionRow) {
					regions.insert(region);
				}
			}
			return regions;
		}

		void Save() {
			for (auto & regionVec : m_regions) {
				for (auto & region : regionVec) {
					region->Save();
				}
			}
			m_entityIndex.Save();
		}
		void Shutdown() {
			for (auto & region : m_loadedRegions) {
				IEventManager::Invoke(EventTypes::WEM_RegionUnloaded, region);
			}
		}
		void ReCenter(float x, float z, float range) {
			int minX = ClampRegion((int)floor((x - range) / m_regionWidth));
			int maxX = ClampRegion((int)ceil((x + range) / m_regionWidth));
			int minZ = ClampRegion((int)floor((z - range) / m_regionWidth));
			int maxZ = ClampRegion((int)ceil((z + range) / m_regionWidth));

			unordered_set<shared_ptr<RegionType>> newRegions;
			for (x = (float)minX; x < (float)maxX; x++) {
				for (z = (float)minZ; z < (float)maxZ; z++) {
					newRegions.insert(m_regions[x][z]);
				}
			}
			auto oldRegions = m_loadedRegions;
			m_loadedRegions = newRegions;
			// Fire unload event for regions going out of scope
			for (auto & oldRegion : oldRegions) {
				if (!newRegions.count(oldRegion)) {
					IEventManager::Invoke(EventTypes::WEM_RegionUnloaded, oldRegion);
				}
			}
			// Fire load event for regions coming into scope
			for (auto & newRegion : newRegions) {
				if (!oldRegions.count(newRegion)) {
					IEventManager::Invoke(EventTypes::WEM_RegionLoaded, newRegion);
				}
			}

			
		
			
			
			// Fire an event for systems to listen to
			IEventManager::Invoke(EventTypes::WEM_Resync);
			//CollectGarbage();
		}
		void CollectGarbage() {
			

			for (auto & regionVector : m_regions) {
				for (auto & region : regionVector) {
					if (!m_loadedRegions.count(region)) {
						// Empty single-reference caches for regions that are no longer loaded
						//TaskManager::Get().Push(Task([=] {
							region->EmptyCache();
						//}));
					}
				}
			}
		}
		bool RegionsLoaded() {
			return m_loadedRegions.size() != 0;
		}
	private:
		//----------------------------------------------------------------
		// Regions
		vector<vector<shared_ptr<RegionType>>> m_regions;
		unordered_set<shared_ptr<RegionType>> m_loadedRegions;

		
		//----------------------------------------------------------------
		// Meta
		const unsigned int m_worldWidth;
		const unsigned int m_regionWidth;
		const unsigned int m_regionDimension;
		const float m_loadRange;
		std::recursive_mutex m_mutex;
		tuple<pair<MaskType, CompTypes>...> m_maskIndex;

		//----------------------------------------------------------------
		// Player
		EntityID m_player;
		WorldEntityCache<RegionType, Position, Player> m_playerCache;
	private:
		//----------------------------------------------------------------
		// Entity indexing
		EntityIndex m_entityIndex;

		//----------------------------------------------------------------
		// Misc

		int ClampRegion(int ordinal) {
			return std::min(m_worldWidth / m_regionWidth, (EntityID)std::max(0, ordinal));
		}
		//----------------------------------------------------------------
		// Region queries
		RegionType & RegionContaining(Vector3 & position) {
			int x;
			int z;
			RegionCoords(position, x, z);
			return *(m_regions[x][z]);
		}
		shared_ptr<RegionType> RegionContaining(EntityID & id) {
			EntityInfo * info;
			if (m_entityIndex.Find(id, info)) {
				return m_regions[info->regionX][info->regionZ];
			}
			return nullptr;
		}
		void RegionCoords(Vector3 & position, int & x, int & z) {
			x = std::max(0, std::min((int)m_regionDimension - 1, (int)position.x / (int)m_regionWidth));
			z = std::max(0, std::min((int)m_regionDimension - 1, (int)position.z / (int)m_regionWidth));
		}
		//----------------------------------------------------------------
		// Loading  entities
		template<typename HeadType, typename ... MaskTypes>
		EntityCache<HeadType, MaskTypes...> LoadEntities(MaskType componentMask) {
			EntityCache<HeadType, MaskTypes...> entityCache;
			for (auto & region : m_loadedRegions) {
				region->LoadEntities<HeadType, MaskTypes...>(entityCache, componentMask);
			}
			return entityCache;
		}
		//----------------------------------------------------------------
		// Set mask
		template <typename HeadType>
		void SetMask(MaskType mask) {
			std::get<pair<MaskType, HeadType>>(m_maskIndex).first = mask;
		}
		template<typename HeadType, typename Next, typename ... MaskTypes>
		void SetMask(MaskType mask = 1) {
			SetMask<HeadType>(mask);
			SetMask<Next, MaskTypes...>(mask * 2);
		}
		//----------------------------------------------------------------
		// Component insertion
		template<typename CompType>
		void InsertComponent(RegionType & region, EntityID & id, MaskType & signature, CompType && component) {
			component.ID = id;
			region.InsertComponent(component, signature);
		}
		template<typename First, typename ... MaskTypes>
		void InsertComponent(RegionType & region, EntityID & id, MaskType & signature, First && first, MaskTypes && ... rest) {
			InsertComponent(region, id, signature, first);
			InsertComponent(region, id, signature, rest...);
		}


	};
}
