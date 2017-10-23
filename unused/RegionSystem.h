#pragma once
#include "System.h"
class RegionSystem :
	public System
{
public:
	RegionSystem(
		shared_ptr<EntityManager> & entityManager,
		vector<string> & components, 
		unsigned short updatePeriod,
		string directory,
		int worldWidth,
		int regionWidth
	);
	~RegionSystem();
protected:
	// Inherited via System
	virtual void Update() override;
private:
	void LoadRegions();
	void SaveEntities(Region & region, unsigned long componentMask);
	string m_directory;

	int m_worldWidth;
	int m_regionWidth;
	vector<vector<Region>> m_regions;

	shared_ptr<Entity> m_player;

	// Inherited via System
	virtual void SyncEntities() override;

	virtual string Name() override;
};

