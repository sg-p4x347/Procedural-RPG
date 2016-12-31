#pragma once
#include <DirectXMath.h>
#include <vector>

#include "Road.h"
#include "Building.h"

using namespace DirectX;
using namespace std;

// Defines cities that lie on the continent
 class City {
 public:
	 City(XMFLOAT3 position,const vector<vector<float>> & terrain);
	 ~City();
	 XMFLOAT3 GetPosition();
 private:
	 // Meta information
	 string m_name;
	 XMFLOAT3 m_position;
	 vector<long> m_citizens;
	 // city construction
	 vector<Road> m_streets;
	 vector<Building> m_buildings;
 };