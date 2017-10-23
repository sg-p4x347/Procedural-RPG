#pragma once
#include "System.h"
#include "HeightMap.h"
#include "Position.h"
#include "VBO.h"
#include "Position.h"
class TerrainSystem :
	public System
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

	void Generate();
	// Inherited via System
	virtual void SyncEntities() override;
	virtual void Update() override;
	virtual string Name() override;
protected:
	//----------------------------------------------------------------
	// Entity helpers
	shared_ptr<Components::Position> PlayerPos();

	//----------------------------------------------------------------
	// Loading and Updating Regions

	// Creates all terrain regions in the world
	void CreateTerrainEntities();
	void UpdateRegions(DirectX::SimpleMath::Vector3 center);
	void UpdateTerrainVBO(shared_ptr<Components::VBO> vbo, int x, int z, int lod);
	int LOD(double distance, unsigned int modelWidth);
	void NewTerrain(DirectX::SimpleMath::Vector3 & position);
	

	// 2 dimensional maps -------------------------------------------
	HeightMap m_terrain;
	HeightMap m_biome;
	int m_width; // The total width of the continent (in meters)
	const int m_regionWidth; // Width of region divisions (in meters)
	Filesystem::path m_directory;

	//----------------------------------------------------------------
	// Generation parameters
	int m_sampleSpacing;
	float m_biomeAmplitude;
	float m_biomeShift;
	float m_biomeWidth;
	float m_continentAmplitude;
	float m_continentShift;
	float m_continentWidth;
	// Diamond Square Algorithm -------------------------------------
	float Diamond(HeightMap & map, int & x, int & y, int & distance);
	float Square(HeightMap & map, int & x, int & y, int & distance);
	float Deviation(float range, float offset = 0.0);
	// Continent Generation -----------------------------------------
	float BiomeDeviation(float biome, float continent);
	float Gaussian(float x, float a, float b, float c); // a controls amplituide; b controls x displacement; c controls width
	float Sigmoid(float x, float a, float b, float c); // a controls amplituide; b controls x displacement; c controls width
	// Erosion filter for generated terrain -------------------------
	void Erosion();
	void SaveTerrain();
};

