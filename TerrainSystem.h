#pragma once
#include <thread>
#include "System.h"
#include "HeightMap.h"
#include "Position.h"
#include "VBO.h"
#include "Position.h"
#include "TerrainCell.h"
using DirectX::SimpleMath::Vector3;
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

	
	// Inherited via System
	virtual void Update(double & elapsed) override;
	virtual string Name() override;
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
	// Entity helpers
	shared_ptr<Components::Position> PlayerPos();
	//----------------------------------------------------------------
	// Threading
	std::thread m_worker;
	//TaskThread m_workerThread;
	//----------------------------------------------------------------
	// Loading and Updating Regions

	// Creates all terrain regions in the world
	void CreateTerrainEntities();
	void UpdateRegions(DirectX::SimpleMath::Vector3 center);
	void UpdateTerrainVBO(shared_ptr<Components::VBO> vbo, int x, int z);
	int LOD(double distance, unsigned int modelWidth);
	void NewTerrain(DirectX::SimpleMath::Vector3 & position);

	// 2 dimensional maps -------------------------------------------
	HeightMap<float> m_terrain;
	HeightMap<float> m_biome;
	HeightMap<TerrainCell> m_erosionMap;
	int m_width; // The total width of the continent (in meters)
	const int m_regionWidth; // Width of region divisions (in meters)
	Filesystem::path m_directory;
	float InternalHeight(std::ifstream & ifs, const int & index);
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
	// Diamond Square Algorithm -------------------------------------
	float Diamond(HeightMap<float> & map, int & x, int & y, int & distance);
	float Square(HeightMap<float> & map, int & x, int & y, int & distance);
	float Deviation(float range, float offset = 0.0);
	// Continent Generation -----------------------------------------
	float BiomeDeviation(float biome, float continent);
	
	// Erosion filter for generated terrain -------------------------
	void Erosion();
	void InitializeErosionMap();
	void SaveTerrain();
};

