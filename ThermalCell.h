#pragma once
#include "HeightMap.h"
#include "Map.h"
/*
Spread model adapted from

"Fast Hydraulic and Thermal Erosion on the GPU"
*/
using DirectX::SimpleMath::Vector2;
class ThermalCell
{
public:
	ThermalCell();
	float DeltaHeight(HeightMap & map, Map<ThermalCell> & terrain,int & x, int &z, int &adjX, int &adjZ);
	float OutFlux[4];
	// Updating the simulation
	void UpdateFlux(HeightMap & map,Map<ThermalCell> & terrain,int &x, int &z);
	void TransferSediment(HeightMap & map, Map<ThermalCell> & terrain,int& x, int &z);

	// Utility
	void Adjacent(const int & i, const int & x, const int & z, int & adjX, int & adjZ);
private:
	static const float m_talusAngle; // angle at which sediment becomes stable
	static const float m_g; // gravity
	static const float m_dt; // simulation time between updates
};

