#pragma once
#include "WorldSystem.h"
#include "HeightMap.h"
#include "WaterCell.h"
#include "WorldDomain.h"
#include "TerrainSystem.h"
class SystemManager;
namespace world {
	class PlantSystem :
		public WorldSystem
	{
	public:
		PlantSystem(SystemManager * systemManger, WEM * entityManager, unsigned short updatePeriod);
		// Inherited via WorldSystem
		virtual string Name() override;
		virtual void Update(double & elapsed) override;
		//----------------------------------------------------------------
		// Public Interface
		void Generate();
	private:
		WorldEntityCache<WEM::RegionType, Position, Plant> m_entities;
		SystemManager * SM;
		//----------------------------------------------------------------
		// Genetic Evolution
		void GenerateGeneticPlants();
		void GenerateOffspring(std::list<shared_ptr<Plant>> & plants);
		void UpdateGeneticPlants(Vector3 lightDirection, std::list<shared_ptr<Plant>> & plants);
		float CalculateLight(Vector3 LightDirection, Plant & plant);
		shared_ptr<DirectX::Model> GenerateModel(Plant & plant);
		void GenerateTopologyRecursive(shared_ptr<Plant::Stem> stem, TopologyCruncher & tc);
		//----------------------------------------------------------------
		// Trees
		void GenerateTreeModels();
		void GenerateTreeEntities(HeightMap & terrain, Map<WaterCell> & water);
		void NewTree(DirectX::SimpleMath::Vector3 & position, Vector3 & rotation);
		float TreeGradientProbability(float gradient);
		float TreeElevationProbability(float elevation);
		//----------------------------------------------------------------
		// Grass
		void GenerateGrassEntities(HeightMap & terrain, Map<WaterCell> & water);
		void NewGrass(DirectX::SimpleMath::Vector3 & position, Vector3 & rotation);
	};

}