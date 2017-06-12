#pragma once
#include <DirectXMath.h>
#include <vector>
#include <map>

#include "SimpleMath.h"
#include "Road.h"
#include "Building.h"
#include "HeightMap.h"

using namespace DirectX;
using namespace SimpleMath;
using namespace std;

// Defines cities that lie on the continent
 class City {
 public:
	 City();
	 City(SimpleMath::Rectangle area,const HeightMap & terrain, const HeightMap & biome);
	 ~City();
	 Vector2 GetPosition();
	 
 private:
	 // Meta information
	 string m_name;
	 SimpleMath::Rectangle m_area;
	 vector<long> m_citizens;
	 // city construction
	 vector<Road> m_streets;
	 vector<Architecture::Building> m_buildings;
	 // biome information
	 map<string, float> m_biomes;
	 // return the name of the biome that this sample point lies in
	 string GetBiomeName(float value);
 };