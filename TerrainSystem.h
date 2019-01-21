#pragma once
#include <thread>
#include "WorldSystem.h"
#include "HeightMap.h"
#include "Position.h"
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
		typedef DirectX::VertexPositionNormalTangentColorTexture VertexType;
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
		shared_ptr<DirectX::Model> GetModel(shared_ptr<world::WEM::RegionType> region);
		void Generate(std::function<void(float,std::string)> && progressCallback);
		string GetBiomeName(float sample);
		float Height(const int & x, const int & z);
		float Height(float & x, float & z);
		float Height(Vector2 position);
		float Biome(const int & x, const int & z);
		int Width();
		DirectX::SimpleMath::Rectangle Area();
		float Flatten(Rectangle area, const int margin = 10);
		void SetHeight(const int x, const int z, float value);
		float Average(Rectangle area);
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
		std::mutex m_mutex;
		std::mutex m_updateLodMutex;
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
		
		Map<shared_ptr<HeightMap>> m_chunks;
		Map<shared_ptr<Map<Vector3>>> m_normals;
		Map<shared_ptr<DirectX::Model>> m_chunkModels;
		Map<int> m_chunkLOD;
		void UpdateLOD(Vector3 center);
		map<shared_ptr<WEM::RegionType>,shared_ptr<HeightMap>> m_cache;
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

		void UpdateDroplets(HeightMap & terrain, shared_ptr<vector<Droplet>> droplets, shared_ptr<Map<ThermalCell>> thermal, std::function<void(float)> && progressCallback);
		void UpdateWater(HeightMap & terrain, Map<WaterCell> & water, std::function<void(float)> && progressCallback);
		//----------------------------------------------------------------
		// Updating meshes
		//void UpdateRegions(DirectX::SimpleMath::Vector3 center);
		//shared_ptr<HeightMap> UpdateTerrainVBO(shared_ptr<Components::PositionNormalTextureTangentColorVBO> vbo, int  x, int  z);
		//void UpdateWaterVBO(shared_ptr<Components::PositionNormalTextureTangentColorVBO> vbo, shared_ptr<HeightMap> terrain, int  x, int z);
		//VertexPositionNormalTangentColorTexture CreateVertex(Vector3 position, Vector3 normal, Vector2 texture);
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