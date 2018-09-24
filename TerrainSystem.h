#pragma once
#include <thread>
#include "WorldSystem.h"
#include "HeightMap.h"
#include "Position.h"
#include "PositionNormalTextureTangentColorVBO.h"
#include "Position.h"
#include "WaterCell.h"
#include "Droplet.h"
#include "ThermalCell.h"
#include "WorldDomain.h"
class SystemManager;

using DirectX::SimpleMath::Vector3;
namespace world {
	class TerrainSystem :
		public WorldSystem
	{
	public:
		TerrainSystem(
			SystemManager * systemManager,
			WEM * entityManager,
			unsigned short updatePeriod,
			int worldWidth,
			int regionWidth,
			Filesystem::path directory
		);
		~TerrainSystem();

		// Inherited via System
		virtual void Update(double & elapsed) override;
		virtual string Name() override;
		virtual void SyncEntities() override;
		//----------------------------------------------------------------
		// Public interface
		void Generate();
		string GetBiomeName(float sample);
		float Height(const int & x, const int & z);
		float Height(float & x, float & z);
		float Biome(const int & x, const int & z);
		int Width();
		DirectX::SimpleMath::Rectangle Area();
		void ImportMap(HeightMap & map);
		float Flatten(HeightMap & map, Rectangle area, const int margin = 10);
		float Height(HeightMap & map, int x, int z);
		float Average(HeightMap & map, Rectangle area);
		void Save(HeightMap & map);
		void RegisterHandlers();
		// terrain manipulation (TEMP currently only updates the VBO)
		void SetVertex(const int & x, const int & z, const float value);
	public:
		// TEMP
		unique_ptr<Map<WaterCell>> WaterMap;
		unique_ptr<HeightMap> TerrainMap;
	protected:
		SystemManager * SM;
		//----------------------------------------------------------------
		// Entities
		//----------------------------------------------------------------
		// Threading
		//Map<std::thread> m_workers;
		//std::thread m_workers[64][64];
		//----------------------------------------------------------------
		// Loading and Updating Regions
		int LOD(double distance, unsigned int modelWidth);
		//----------------------------------------------------------------
		// Terrain cache
		shared_ptr<HeightMap> m_cache;
		Vector2 m_cachePos;
		void UpdateCache(Vector3 center);
		// 2 dimensional maps -------------------------------------------
		int m_width;				// The total width of the continent (in meters)
		const int m_regionWidth;	// Width of region divisions (in meters)
		Filesystem::path m_directory;
		float InternalHeight(std::ifstream & ifs, const int & index, float precision);
		Vector3 Normal(std::ifstream & ifs, const int & index);

		//----------------------------------------------------------------
		// Generation parameters
		int m_sampleSpacing;
		float m_biomeAmplitude;
		float m_biomeShift;
		float m_biomeWidth;
		float m_continentAmplitude;
		float m_continentShift;
		float m_continentWidth;
		//----------------------------------------------------------------
		// Diamond Square Algorithm
		float Diamond(HeightMap & map, int & x, int & y, int & distance);
		float Square(HeightMap & map, int & x, int & y, int & distance);
		float Deviation(float range, float offset = 0.0);
		//----------------------------------------------------------------
		// Continent Generation
		float BiomeDeviation(float biome, float continent);

		//----------------------------------------------------------------
		// Droplet and Thermal based erosion
		void InitializeErosionMap();
		shared_ptr<Map<ThermalCell>> InitializeThermalErosionMap();
		shared_ptr<vector<Droplet>> InitializeDroplets();

		void UpdateDroplets(HeightMap & terrain, shared_ptr<vector<Droplet>> droplets, shared_ptr<Map<ThermalCell>> thermal);
		void UpdateWater(HeightMap & terrain, Map<WaterCell> & water);
		//----------------------------------------------------------------
		// Updating meshes
		//void UpdateRegions(DirectX::SimpleMath::Vector3 center);
		shared_ptr<HeightMap> UpdateTerrainVBO(shared_ptr<Components::PositionNormalTextureTangentColorVBO> vbo, int  x, int  z);
		void UpdateWaterVBO(shared_ptr<Components::PositionNormalTextureTangentColorVBO> vbo, shared_ptr<HeightMap> terrain, int  x, int z);
		VertexPositionNormalTangentColorTexture CreateVertex(Vector3 position, Vector3 normal, Vector2 texture);
		float LowestNeighbor(HeightMap & water, HeightMap & terrain, int x, int z);
		//----------------------------------------------------------------
		// Terrain
		void CreateTerrainEntities();
		void NewTerrain(DirectX::SimpleMath::Vector3 & position);
		void SaveTerrain(HeightMap & terrain, HeightMap & biome);
		//----------------------------------------------------------------
		// Water
		void CreateWaterEntities();
		void NewWater(DirectX::SimpleMath::Vector3 & position);
		void SaveWater(Map<WaterCell> & water);
		//----------------------------------------------------------------
		// Resources
		//void CreateResourceEntities();
		//void NewResourceNode(Vector3 & position, Vector3 & rotation, string model, std::map<string, int> inventoryItems);
		float ResourceGradientProbability(float gradient);
	};

}