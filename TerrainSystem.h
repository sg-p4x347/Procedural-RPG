#pragma once
#include <thread>
#include "WorldSystem.h"
#include "HeightMap.h"
#include "Position.h"
#include "VBO.h"
#include "Position.h"
#include "WaterCell.h"
#include "Droplet.h"
#include "ThermalCell.h"

using DirectX::SimpleMath::Vector3;
class TerrainSystem :
	public WorldSystem
{
public:
	TerrainSystem(
		shared_ptr<EntityManager> & entityManager, 
		vector<string> & components, 
		unsigned short updatePeriod, 
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

	float Biome(const int & x, const int & z);
	int Width();
	DirectX::SimpleMath::Rectangle Area();

	// terrain manipulation (TEMP currently only updates the VBO)
	void SetVertex(const int & x, const int & z, const float value);
protected:
	//----------------------------------------------------------------
	// Entities
	shared_ptr<Components::Position> PlayerPos();
	vector<EntityPtr> m_terrainEntities;
	vector<EntityPtr> m_waterEntities;
	//----------------------------------------------------------------
	// Threading
	std::thread m_worker;
	//----------------------------------------------------------------
	// Loading and Updating Regions
	int LOD(double distance, unsigned int modelWidth);
	

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
	shared_ptr<Map<WaterCell>> InitializeErosionMap();
	shared_ptr<Map<ThermalCell>> InitializeThermalErosionMap();
	shared_ptr<vector<Droplet>> InitializeDroplets();

	void UpdateDroplets(HeightMap & terrain, shared_ptr<vector<Droplet>> droplets, shared_ptr<Map<ThermalCell>> thermal);
	void UpdateWater(HeightMap & terrain, shared_ptr<Map<WaterCell>> water);
	//----------------------------------------------------------------
	// Updating meshes
	void UpdateRegions(DirectX::SimpleMath::Vector3 center);
	shared_ptr<HeightMap> UpdateTerrainVBO(shared_ptr<Components::VBO> vbo, int  x, int  z);
	void UpdateWaterVBO(shared_ptr < Components::VBO> vbo, shared_ptr<HeightMap> terrain, int  x, int z);
	VertexPositionNormalTangentColorTexture CreateVertex(Vector3 position, Vector3 normal, Vector2 texture);
	float LowestNeighbor(HeightMap & water,HeightMap & terrain, int x, int z);
	//----------------------------------------------------------------
	// Terrain
	void CreateTerrainEntities();
	void NewTerrain(DirectX::SimpleMath::Vector3 & position);
	void SaveTerrain(HeightMap & terrain, HeightMap & biome);
	//----------------------------------------------------------------
	// Water
	void CreateWaterEntities();
	void NewWater(DirectX::SimpleMath::Vector3 & position);
	void SaveWater(shared_ptr<Map<WaterCell>> water);
	//----------------------------------------------------------------
	// Trees
	void CreateTreeEntities(HeightMap & terrain, shared_ptr<Map<WaterCell>> water);
	void NewTree(DirectX::SimpleMath::Vector3 & position,Vector3 & rotation);
	float TreeGradientProbability(float gradient);
	float TreeElevationProbability(float elevation);
};

