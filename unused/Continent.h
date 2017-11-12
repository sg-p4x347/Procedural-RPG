#pragma once
#include "Utility.h"
#include "HeightMap.h"
#include "City.h"
#include "Federal.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace Utility;
using namespace History;

class Continent
{
public:
	Continent(string directory);
	~Continent();
	
	
	// Getters
	const HeightMap & GetTerrain() const;
	const HeightMap & GetBiome() const;
	const int & GetWidth() const;
	
private:
	// Saving data to files -----------------------------------------
	void Save(string directory);
	// 2 dimensional maps -------------------------------------------
	HeightMap m_terrain;
	HeightMap m_biome;
	int m_width; // The total width of the continent (in meters)
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
	
	void Load();
};

