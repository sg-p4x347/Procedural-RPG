#pragma once
#include "HeightMap.h"
#include "Map.h"
#include "Stratum.h"
/*
Spread model adapted from

"Fast Hydraulic Erosion Simulation and Visualization on GPU"

Xing Mei, Philippe Decaudin, Bao-Gang Hu. Fast Hydraulic Erosion Simulation and Visualization on
GPU. Marc Alexa and Steven J. Gortler and Tao Ju. PG ’07 - 15th Pacific Conference on Computer
Graphics and Applications, Oct 2007, Maui, United States. IEEE, pp.47-56, 2007, Pacific Graphics
2007. <10.1109/PG.2007.15>. <inria-00402079>
*/
using DirectX::SimpleMath::Vector3;
class WaterCell
{
public:
	WaterCell();
	float Water;
	float DeltaHeight(HeightMap & map, Map<WaterCell> & water, int & x, int & z,int & adjX, int & adjZ);
	float OutFlux[4];

	// Updating the simulation
	void Rain();
	void UpdateFlux(HeightMap & map, Map<WaterCell> & water, int & x, int & z);
	void UpdateWater(HeightMap & map, Map<WaterCell> & water, int & x, int & z);
	void Evaporate();

	// Utility
	void Adjacent(const int & i,const int & x, const int & z, int & adjX, int & adjZ);
private:
	static const float rainQty;
	static const float rainProbability;
	static const float evaporation;
	static const float g;
	static const float dt;
};

