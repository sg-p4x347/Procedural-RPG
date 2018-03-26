#pragma once
#include "WorldSystem.h"
#include "HeightMap.h"
#include "WaterCell.h"
class SystemManager;
class PlantSystem :
	public WorldSystem
{
public:
	PlantSystem(SystemManager * systemManger, unique_ptr<WorldEntityManager> & entityManager, vector<string> & components, unsigned short updatePeriod);
	// Inherited via WorldSystem
	virtual string Name() override;
	virtual void Update(double & elapsed) override;
	//----------------------------------------------------------------
	// Public Interface
	void Generate();
private:
	SystemManager * SM;

	//----------------------------------------------------------------
	// Trees
	void GenerateTreeModels();
	void GenerateTreeEntities(HeightMap & terrain, Map<WaterCell> & water);
	void NewTree(DirectX::SimpleMath::Vector3 & position, Vector3 & rotation);
	float TreeGradientProbability(float gradient);
	float TreeElevationProbability(float elevation);
};

