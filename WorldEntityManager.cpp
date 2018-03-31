#include "pch.h"
#include "WorldEntityManager.h"
#include "JsonParser.h"
#include "IEventManager.h"
#include "PositionNormalTextureTangentColorVBO.h"
#include "Inventory.h"
WorldEntityManager::WorldEntityManager(Filesystem::path directory, int worldWidth, int minQuadWidth) : PersistenceEntityManager::PersistenceEntityManager(directory), m_player(nullptr), m_quadTree(Rectangle(0,0,worldWidth,worldWidth),minQuadWidth)
{
	//----------------------------------------------------------------
	// Register the components
	RegisterComponent([] {return new Components::Player();});
	RegisterComponent([] {return new Components::Position();});
	RegisterComponent([] {return new Components::Movement();});
	RegisterComponent([] {return new Components::Collision();});
	RegisterComponent([] {return new Components::Terrain();});
	RegisterComponent([] {return new Components::PositionNormalTextureVBO();});
	RegisterComponent([] {return new Components::PositionNormalTextureTangentColorVBO(); });
	RegisterComponent([] {return new Components::Model();});
	RegisterComponent([] {return new Components::Action();});
	RegisterComponent([] {return new Components::EntityRegion();});
	RegisterComponent([] {return new Components::Building();});
	RegisterComponent([] {return new Components::Inventory();});
	//----------------------------------------------------------------
	// Tags
	RegisterDelegate([](string type) {return new Components::Tag(type);}, vector<string>{
		"Water",
		"Tree"
	});
	//----------------------------------------------------------------
	// Parse configuration file
	JsonParser config = JsonParser(std::ifstream("config/WorldEntityManager.json"));
	m_entityRegionWidth = (unsigned int)config["EntityRegionWidth"];
	JsonParser continentConfig = JsonParser(std::ifstream("config/continent.json"));
	m_worldWidth = (unsigned int)continentConfig["terrainMap"]["width"];

	

	//----------------------------------------------------------------
	// Event Handlers
	IEventManager::RegisterHandler(Entity_ComponentAdded, std::function<void(unsigned int, unsigned long)>([=](unsigned int target, unsigned long mask) {
		if (mask == ComponentMask("Position")) {
			AddEntityToRegion(target);
		}
	}));
	//----------------------------------------------------------------
	// Cache entity regions
	auto regions = FindEntities(ComponentMask(vector<string>{"EntityRegion", "Position"}));
	m_entityRegions = std::set<EntityPtr>(regions.begin(),regions.end());
}
vector<EntityPtr> WorldEntityManager::FindEntitiesInRange(unsigned long componentMask, Vector3 center, float range)
{
	SyncRegions(center, m_entityRegionWidth);
	vector<EntityPtr> results;
	Rectangle searchArea = Rectangle(center.x - range, center.z - range, range * 2, range * 2);
	// Filter by position
	for (auto & entityID : m_quadTree.Find(searchArea)) {
		EntityPtr entity;
		if (Find(entityID, entity)) {
			// Filter by mask
			if (entity->HasComponents(componentMask)) {
				results.push_back(entity);
			}
		}
	}
	return results;
	/*auto entities = FindEntities(componentMask | ComponentMask("Position"));
	vector<EntityPtr> finalSet;
	for (auto & entity : entities) {
		if (Vector3::Distance(entity->GetComponent<Components::Position>("Position")->Pos, center) <= range) {
			finalSet.push_back(entity);
		}
	}
	return finalSet;*/
}

//vector<EntityPtr> WorldEntityManager::EntitiesByRegion(Vector3 center, float range)
//{
//	center.y = 0; // Ignore any vertical distance
//	vector<EntityPtr> entities;
//	vector<EntityPtr> entityRegions = FindEntities(ComponentMask(vector<string>{"EntityRegion", "Position"}));
//	for (EntityPtr & region : entityRegions) {
//		Vector3 regionPosition = region->GetComponent<Components::Position>("Position")->Pos;
//		if (Vector3::Distance(center, regionPosition) <= range) {
//			auto regionComp = region->GetComponent<Components::EntityRegion>("EntityRegion");
//			for (unsigned int & entity : regionComp->Entities) {
//				EntityPtr target;
//				if (Find(entity, target)) entities.push_back(target);
//			}
//		}
//	}
//	return entities;
//}

vector<EntityPtr> WorldEntityManager::Filter(vector<EntityPtr>&& entities, unsigned long componentMask)
{
	vector<EntityPtr> results;
	for (EntityPtr & entity : entities) {
		if (entity->HasComponents(componentMask)) results.push_back(entity);
	}
	return results;
}

void WorldEntityManager::AddEntityToRegion(EntityPtr entity)
{
	AddEntityToRegion(entity->ID());
}

void WorldEntityManager::GenerateEntityRegions()
{
	const unsigned int dimension = m_worldWidth / m_entityRegionWidth;
	for (int regionX = 0; regionX < dimension; regionX++) {
		for (int regionZ = 0; regionZ < dimension; regionZ++) {
			EntityPtr region = NewEntity();
			region->AddComponent(new Components::Position(Vector3(((float)regionX + 0.5) * (float)m_entityRegionWidth, 0.f, ((float)regionZ + 0.5) * (float)m_entityRegionWidth)));
			region->AddComponent(new Components::EntityRegion(m_entityRegionWidth));
			m_entityRegions.insert(region);
		}
	}
}

void WorldEntityManager::AddEntityToRegion(unsigned int entity)
{
	EntityPtr target;
	if (Find(entity, target)) {
		Vector3 entityPosition = target->GetComponent<Components::Position>("Position")->Pos;
		for (const EntityPtr & region : m_entityRegions) {
			Rectangle regionArea = RegionArea(region);
			if (regionArea.Contains(entityPosition.x,entityPosition.z)) {
				region->GetComponent<Components::EntityRegion>("EntityRegion")->AddEntity(entity);
			}
		}
	}
}

void WorldEntityManager::SyncRegions(Vector3 center, float range)
{
	Rectangle selectionRect = Rectangle(center.x - range, center.z - range, range * 2, range * 2);
	
	
	set<EntityPtr> newRegions;
	for (const EntityPtr & region : m_entityRegions) {
		Vector3 regionPosition = region->GetComponent<Components::Position>("Position")->Pos;
		if (selectionRect.Contains(regionPosition.x, regionPosition.z)) {
			newRegions.insert(region);
		}
	}
	// Only sync if the two sets are not equal
	if (newRegions != m_loadedEntityRegions) {
		// uncache entities that fall outside of the new region set
		for (auto & region : m_loadedEntityRegions) {
			if (newRegions.count(region) == 0) {
				m_quadTree.Remove(RegionArea(region));
			}
		}
		// cache entities in regions that haven't been loaded yet
		for (auto & region : newRegions) {
			if (m_loadedEntityRegions.count(region) == 0) {
				// Add the entities to the quadTree
				for (auto & entityID : region->GetComponent<Components::EntityRegion>("EntityRegion")->Entities) {
					EntityPtr entity;
					if (Find(entityID, entity)) {
						Vector3 pos3D = entity->GetComponent<Components::Position>("Position")->Pos;
						Vector2 pos2D = Vector2(pos3D.x, pos3D.z);
						m_quadTree.Insert(pos2D, entityID);
					}
				}
				m_loadedEntityRegions.insert(region);
			}
		}
		// load the new regions
		m_loadedEntityRegions = newRegions;
	}
}

Rectangle WorldEntityManager::RegionArea(EntityPtr region)
{
	Vector3 regionPosition = region->GetComponent<Components::Position>("Position")->Pos;
	shared_ptr<Components::EntityRegion> entityRegion = region->GetComponent<Components::EntityRegion>("EntityRegion");
	int halfWidth = entityRegion->RegionWidth * 0.5f;
	return Rectangle(regionPosition.x - halfWidth,regionPosition.z - halfWidth, entityRegion->RegionWidth, entityRegion->RegionWidth);
}

EntityPtr WorldEntityManager::Player()
{
	if (!m_player) {
		auto results = FindEntities(ComponentMask("Player"));
		m_player = results.size() != 0 ? results[0] : nullptr;
		if (!m_player) {
			Utility::OutputException("Player data missing");
		}
	}
	return m_player;
}

shared_ptr<Components::Position> WorldEntityManager::PlayerPos()
{
	return Player()->GetComponent<Components::Position>("Position");
}


//----------------------------------------------------------------
// Sectors

