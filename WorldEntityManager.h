#pragma once
#include "PersistenceEntityManager.h"
#include "EntityQuadTree.h"
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
	WorldEntityManager(Filesystem::path directory, int worldWidth, int minQuadWidth);
	//----------------------------------------------------------------
	// Entity queries
	EntityPtr Player();
	shared_ptr<Components::Position> PlayerPos();

	vector<EntityPtr> FindEntitiesInRange(unsigned long componentMask, Vector3 center, float range);
	//----------------------------------------------------------------
	// Returns entities within the immediate entity regions
	//vector<EntityPtr> EntitiesByRegion(Vector3 center, float range);
	void AddEntityToRegion(EntityPtr entity);
	void GenerateEntityRegions();
	//----------------------------------------------------------------
	// Filter a vector of entities with a mask
	vector<EntityPtr> Filter(vector<EntityPtr> && entities, unsigned long componentMask);

	
private:
	// Commonly used
	EntityPtr m_player;

	//----------------------------------------------------------------
	// Configuration
	unsigned int m_entityRegionWidth;
	unsigned int m_worldWidth;
	
	void AddEntityToRegion(unsigned int entity);
	//----------------------------------------------------------------
	// Caching
	void SyncRegions(Vector3 center, float range);
	Rectangle RegionArea(EntityPtr region);
	set<EntityPtr> m_entityRegions;
	set<EntityPtr> m_loadedEntityRegions;
	EntityQuadTree m_quadTree;

};

