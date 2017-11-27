#pragma once
/*
Spread model adapted from

"Fast Hydraulic Erosion Simulation and Visualization on GPU"

Xing Mei, Philippe Decaudin, Bao-Gang Hu. Fast Hydraulic Erosion Simulation and Visualization on
GPU. Marc Alexa and Steven J. Gortler and Tao Ju. PG ’07 - 15th Pacific Conference on Computer
Graphics and Applications, Oct 2007, Maui, United States. IEEE, pp.47-56, 2007, Pacific Graphics
2007. <10.1109/PG.2007.15>. <inria-00402079>
*/
using DirectX::SimpleMath::Vector3;
class TerrainCell
{
public:
	TerrainCell();
	float Volume();

	Vector3 Velocity;
	float Water;
	float Soil;
	float Suspended;
	float NextSuspended;
	float Height;
	float TotalFlux;
	float Gradient;
	float DeltaHeight(TerrainCell & adjacent);
	void UpdateGradient();
	float OutFlux[4];
	TerrainCell* Adjacent[4];
	// Updating the simulation
	void ApplyDeltas();
	void Rain(float t);
	void UpdateFlux(float t);
	void UpdateWater(float t);
	void Erode(float t);
	void Deposit(float t);
	void Transport(float t);
	void Evaporate(float t);
private:
	static const float rainHeight;
	static const float rainQty;
	static const float g;
	static const float maxDissolved;
};

