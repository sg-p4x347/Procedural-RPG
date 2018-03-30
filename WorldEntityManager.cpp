#include "pch.h"
#include "WorldEntityManager.h"
#include "JsonParser.h"
#include "IEventManager.h"
#include "PositionNormalTextureTangentColorVBO.h"
#include "Inventory.h"
WorldEntityManager::WorldEntityManager(Filesystem::path directory) : PersistenceEntityManager::PersistenceEntityManager(directory), m_player(nullptr)
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

	GenerateEntityRegions();

	//----------------------------------------------------------------
	// Event Handlers
	/*IEventManager::RegisterHandler(Entity_ComponentAdded, std::function<void(unsigned int, unsigned long)>([=](unsigned int target, unsigned long mask) {
		if (mask == ComponentMask("Position")) {
			AddEntityToRegion(target);
		}
	}));*/
}
vector<EntityPtr> WorldEntityManager::FindEntitiesInRange(unsigned long componentMask, Vector3 center, float range)
{
	auto entities = FindEntities(componentMask | ComponentMask("Position"));
	vector<EntityPtr> finalSet;
	for (auto & entity : entities) {
		if (Vector3::Distance(entity->GetComponent<Components::Position>("Position")->Pos, center) <= range) {
			finalSet.push_back(entity);
		}
	}
	return finalSet;
}

vector<EntityPtr> WorldEntityManager::EntitiesByRegion(Vector3 center, float range)
{
	center.y = 0; // Ignore any vertical distance
	vector<EntityPtr> entities;
	vector<EntityPtr> entityRegions = FindEntities(ComponentMask(vector<string>{"EntityRegion", "Position"}));
	for (EntityPtr & region : entityRegions) {
		Vector3 regionPosition = region->GetComponent<Components::Position>("Position")->Pos;
		if (Vector3::Distance(center, regionPosition) <= range) {
			auto regionComp = region->GetComponent<Components::EntityRegion>("EntityRegion");
			for (unsigned int & entity : regionComp->Entities) {
				EntityPtr target;
				if (Find(entity, target)) entities.push_back(target);
			}
		}
	}
	return entities;
}

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

void WorldEntityManager::MoveEntity(EntityPtr entity, EntityPtr source, EntityPtr target)
{
	
}

void WorldEntityManager::GenerateEntityRegions()
{
	const int dimension = m_worldWidth / m_entityRegionWidth;
	for (int regionX = 0; regionX < dimension; regionX++) {
		for (int regionZ = 0; regionZ < dimension; regionZ++) {
			EntityPtr region = NewEntity();
			region->AddComponent(new Components::Position(Vector3(((float)regionX + 0.5) * (float)m_entityRegionWidth, 0.f, ((float)regionZ + 0.5) * (float)m_entityRegionWidth)));
			region->AddComponent(new Components::EntityRegion(m_entityRegionWidth));
		}
	}
}

void WorldEntityManager::AddEntityToRegion(unsigned int entity)
{
	EntityPtr target;
	if (Find(entity, target)) {
		Vector3 entityPosition = target->GetComponent<Components::Position>("Position")->Pos;
		vector<EntityPtr> entityRegions = FindEntities(ComponentMask(vector<string>{"EntityRegion", "Position"}));
		for (EntityPtr & region : entityRegions) {
			Vector3 regionPosition = region->GetComponent<Components::Position>("Position")->Pos;
			if (entityPosition.x > regionPosition.x - (float)m_entityRegionWidth * 0.5f &&
				entityPosition.x < regionPosition.x + (float)m_entityRegionWidth * 0.5f &&
				entityPosition.z > regionPosition.z - (float)m_entityRegionWidth * 0.5f &&
				entityPosition.z < regionPosition.z + (float)m_entityRegionWidth * 0.5f
				) {
				region->GetComponent<Components::EntityRegion>("EntityRegion")->Entities.push_back(entity);
			}
		}
	}
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

