#pragma once
#include "Position.h"
#include "Player.h"
#include "Region.h"
#include "WorldEntityCache.h"
#include "EntityIndex.h"
#include "WorldEntityProxy.h"
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
			m_playerCache(NewEntityCache<Position,Player>())
		{
			SetMask<CompTypes...>();

			m_regions = vector<vector<shared_ptr<Region<CompTypes...>>>>(m_regionDimension, vector<shared_ptr<Region<CompTypes...>>>(m_regionDimension));

			for (int x = 0; x < m_regionDimension; x++) {
				for (int z = 0; z < m_regionDimension; z++) {
					m_regions[x][z] = make_shared<Region<CompTypes...>>(directory / (std::to_string(x) + ',' + std::to_string(z)), m_maskIndex);
					//m_loadedRegions.insert(m_regions[x][z]);
				}
			}

			// Re-center the loaded regions on the player position
			IEventManager::RegisterHandler(EventTypes::Movement_PlayerMoved, std::function<void(int,int)>([=](int x, int z) {
				ReCenter(x, z, loadRange);
			}));

		}
		~WorldEntityManager() {
			Save();
		}


		EntityID PlayerID() {
			return m_player;
		}
		Vector3 PlayerPos() {
			return GetEntity<Position>(m_player).Get<Position>().Pos;
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

		template<typename ... SigTypes>
		EntityID CreateEntity(Position && position, SigTypes && ... components) {
			MaskType signature = GetMask<Position, SigTypes...>();
			int regionX;
			int regionZ;
			RegionCoords(position.Pos, regionX, regionZ);
			auto & region = *m_regions[regionX][regionZ];
			EntityID id = m_entityIndex.New(EntityInfo(signature, regionX, regionZ));
			InsertComponent(region, id, signature, position, components...);
			return id;
		}
		template<typename ... SigTypes>
		EntityID CreatePlayer(Position && position, SigTypes && ... components) {
			m_player = CreateEntity<SigTypes...>(std::move(position), std::move(components)...);
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
					region->ChangeSignature(id, info->signature, signature);
					info->signature = signature;
					if (!signature) {
						// entity is deleted
						m_entityIndex.Delete(id);
					}
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
		WorldEntityProxy<HeadType, MaskTypes...> GetEntity(EntityID id) {
			std::lock_guard<std::mutex> lock(m_mutex);
			EntityInfo * info;
			if (m_entityIndex.Find(id, info)) {
				auto & region = *m_regions[info->regionX][info->regionZ];
				EntityCache<HeadType, MaskTypes...> entityCache;
				region.LoadEntities(entityCache, info->signature,true);
				for (auto & entity : entityCache) {
					if (entity.GetID() == id)
						return entity.GetProxy();
				}
			}
			assert("Entity not found");
		}

		template<typename HeadType, typename ... MaskTypes>
		void UpdateCache(WorldEntityCache<Region<CompTypes...>, HeadType, MaskTypes...> & cache) {
			std::lock_guard<std::mutex> lock(m_mutex);
			world::MaskType componentMask = cache.GetComponentMask();
			// iterate over the cached regions
			unordered_set<shared_ptr<Region<CompTypes...>>> staleRegions;
			for (auto & regionCache : cache.GetCaches()) {
				if (!m_loadedRegions.count(regionCache.first))
					staleRegions.insert(regionCache.first);
			}
			// remove stale region caches
			for (auto & staleRegion : staleRegions) {
				cache.GetCaches().erase(staleRegion);
			}
			// add new region caches to cache
			for (auto & loadedRegion : m_loadedRegions) {
				if (!cache.GetCaches().count(loadedRegion)) {
					EntityCache<HeadType, MaskTypes...> entityCache;
					loadedRegion->LoadEntities<HeadType, MaskTypes...>(entityCache, componentMask);
					cache.GetCaches().insert(make_pair(loadedRegion, entityCache));
					//cache.GetCaches().insert(make_pair(loadedRegion, LoadEntities<HeadType, MaskTypes...>(cache.GetComponentMask())));
				}
			}
		}

		// Not restricted by the loaded region subset
		template<typename HeadType, typename ... MaskTypes>
		void UpdateGlobalCache(WorldEntityCache<Region<CompTypes...>, HeadType, MaskTypes...> & cache) {
			std::lock_guard<std::mutex> lock(m_mutex);
			world::MaskType componentMask = cache.GetComponentMask();
			for (auto & regionRow : m_regions) {
				for (auto & region : regionRow) {
					if (!cache.GetCaches().count(region)) {
						EntityCache<HeadType, MaskTypes...> entityCache;
						region->LoadEntities<HeadType, MaskTypes...>(entityCache, componentMask);
						cache.GetCaches().insert(make_pair(region, entityCache));
					}
				}
			}
		}

		RegionType & GetRegion(int x, int z) {
			return *(m_regions[x][z]);
		}

		void Save() {
			for (auto & region : m_loadedRegions) {
				region->Save();
			}
			m_entityIndex.Save();
		}
		void ReCenter(float x, float z, float range) {
			int minX = ClampRegion((int)floor((x - range) / m_regionWidth));
			int maxX = ClampRegion((int)ceil((x + range) / m_regionWidth));
			int minZ = ClampRegion((int)floor((z - range) / m_regionWidth));
			int maxZ = ClampRegion((int)ceil((z + range) / m_regionWidth));

			auto oldSet = m_loadedRegions;
			m_loadedRegions.clear();
			for (x = minX; x < maxX; x++) {
				for (z = minZ; z < maxZ; z++) {
					m_loadedRegions.insert(m_regions[x][z]);
				}
			}
			// Empty caches for regions that are no longer loaded
			for (auto & old : oldSet) {
				if (!m_loadedRegions.count(old)) {
					old->EmptyCache();
				}
			}
			// Fire an event for systems to listen to
			IEventManager::Invoke(EventTypes::WEM_Resync);
		}
		bool RegionsLoaded() {
			return m_loadedRegions.size() != 0;
		}
	private:
		vector<vector<shared_ptr<Region<CompTypes...>>>> m_regions;
		unordered_set<shared_ptr<Region<CompTypes...>>> m_loadedRegions;
		tuple<pair<MaskType, CompTypes>...> m_maskIndex;
		const unsigned int m_worldWidth;
		const unsigned int m_regionWidth;
		const unsigned int m_regionDimension;
		std::mutex m_mutex;
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
			x = std::max(0, std::min((int)m_regionDimension - 1, int(position.x / m_regionWidth)));
			z = std::max(0, std::min((int)m_regionDimension - 1, int(position.z / m_regionWidth)));
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
