#pragma once
#include "PersistenceEntityManager.h"
//----------------------------------------------------------------
// World Components
#include "Position.h"
#include "Player.h"
#include "PositionNormalTextureVBO.h"
#include "Movement.h"
#include "Collision.h"
#include "Tag.h"
#include "Terrain.h"
#include "Model.h"
#include "Action.h"
#include "EntityRegion.h"
#include "Building.h"
class WorldEntityManager :
	public PersistenceEntityManager
{
public:
	WorldEntityManager(Filesystem::path directory);
	//----------------------------------------------------------------
	// Entity queries
	EntityPtr Player();
	shared_ptr<Components::Position> PlayerPos();

	vector<EntityPtr> FindEntitiesInRange(unsigned long componentMask, Vector3 center, float range);
	//----------------------------------------------------------------
	// Returns entities within the immediate entity regions
	vector<EntityPtr> EntitiesByRegion(Vector3 center, float range);
	//----------------------------------------------------------------
	// Filter a vector of entities with a mask
	vector<EntityPtr> Filter(vector<EntityPtr> && entities, unsigned long componentMask);

	void AddEntityToRegion(EntityPtr entity);
	//----------------------------------------------------------------
	// Sectors
	void MoveEntity(EntityPtr entity, EntityPtr source, EntityPtr target);
private:
	// Commonly used
	EntityPtr m_player;

	//----------------------------------------------------------------
	// Configuration
	unsigned int m_entityRegionWidth;
	unsigned int m_worldWidth;
	void GenerateEntityRegions();
	void AddEntityToRegion(unsigned int entity);
	

};

