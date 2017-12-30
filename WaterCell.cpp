#include "pch.h"
#include "WaterCell.h"
#include "Utility.h"
#include "Stratum.h"
// Constants
const float WaterCell::rainQty = 0.01f;
const float WaterCell::rainProbability = 1.f;
const float WaterCell::evaporation = 0.001f;
const float WaterCell::g = 9.8;
const float WaterCell::dt = 0.1f;
WaterCell::WaterCell() : OutFlux{ 0.f,0.f,0.f,0.f }, Water(0.f)
{
}


float WaterCell::DeltaHeight(HeightMap & map, Map<WaterCell> & water, int & x, int & z, int & adjX, int & adjZ)
{
	return (map.map[x][z] + Water) - (map.map[adjX][adjZ] + water.map[adjX][adjZ].Water);
}


void WaterCell::Rain()
{
	//Suspended += 0.1;
	static const float dw = rainQty * dt;
	if (Utility::Chance(rainProbability)) {
		Water += dw;
	}
}

void WaterCell::Evaporate()
{
	//Water = std::max(0.f, Water - evaporationConstant*t);
	static const float dw = evaporation * dt;
	Water = std::max(0.f,Water-dw);
	//Water *= (1.f - evaporation);
}

void WaterCell::Adjacent(const int & i, const int & x, const int & z, int & adjX, int & adjZ)
{
	adjX = x;
	adjZ = z;
	switch (i) {
	case 0: adjX++; break;
	case 1: adjZ++;break;
	case 2: adjX--;break;
	case 3: adjZ--;break;
	}
}

void WaterCell::UpdateFlux(HeightMap & map, Map<WaterCell> & water, int & x, int & z)
{
	
	// calculate initial flux
	float totalFlux = 0.f;
	// Correct formula *****************************************************************
	//float length = 1;
	//float crossSectionalArea = length * length; //(Water + adjacent.Water) / 2.f;
	//float fluxFactor = (t * crossSectionalArea * g) / length;
	//**********************************************************************************

	// Stream lined **********
	static const float fluxFactor = dt * g;
	//***********************
	for (int i = 0; i < 4; i++) {
		int adjX;
		int adjZ;
		Adjacent(i, x, z, adjX, adjZ);
		if (water.Bounded(adjX, adjZ)) {
			OutFlux[i] = std::max(0.f,
				OutFlux[i] + DeltaHeight(map, water, x, z, adjX, adjZ)* fluxFactor);
			totalFlux += OutFlux[i];

		}
		else {
			OutFlux[i] = 0.f;
		}
	}
	
	// scale down proportionate to water volume
	if (totalFlux > 0.01f && totalFlux > Water) {
		float scaleFactor = std::min(1.f, std::abs(Water / (totalFlux * dt)));
		for (int i = 0; i < 4; i++) {
			OutFlux[i] *= scaleFactor;
		}
	}
}

void WaterCell::UpdateWater(HeightMap & map, Map<WaterCell> & water, int & x, int & z)
{
	// calculate total in and out flux
	float totalInFlux = 0.f;
	float totalOutFlux = 0.f;

	for (int i = 0; i < 4; i++) {
		int adjX;
		int adjZ;
		Adjacent(i, x, z, adjX, adjZ);
		if (water.Bounded(adjX, adjZ)) {
			int adjIndex = (i + 2) % 4;
			totalInFlux += water.map[adjX][adjZ].OutFlux[adjIndex];
			totalOutFlux += OutFlux[i];
		}
	}
	float deltaVolume = dt * (totalInFlux - totalOutFlux);
	Water += deltaVolume;
}

