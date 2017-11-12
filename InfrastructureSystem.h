#pragma once
#include "System.h"
#include "TerrainSystem.h"

#include "Position.h"
#include "City.h"
#include "Building.h"

class InfrastructureSystem :
	public System
{
public:
	InfrastructureSystem(shared_ptr<EntityManager> & entityManager, vector<string> & components, unsigned short updatePeriod);
	~InfrastructureSystem();

	// Inherited via System
	virtual string Name() override;
	virtual void Update(double & elapsed) override;
	virtual void SyncEntities() override;
	//----------------------------------------------------------------
	// Public interface
	void Generate();
	void SetTerrainSystem(shared_ptr<TerrainSystem> & terrainSystem);
private:
	//----------------------------------------------------------------
	// Data
	shared_ptr<TerrainSystem> TS;
	vector<shared_ptr<Entity>> m_cities;
	//----------------------------------------------------------------
	// Function
	void CreateCity();
};

