#include "pch.h"
#include "TerrainCell.h"
#include "Utility.h"
// water
const float TerrainCell::rainHeight = 8.f;
const float TerrainCell::rainQty = 10.f;
const float TerrainCell::g = 9.8;
const float TerrainCell::maxDissolved = 0.3;
static const float rainProbability = 1.f;
static const float evaporationConstant = 0.2f;
static const float damping = 1.f;
static const float viscocity = 8.f;
// erosion
const float transferConstant = 1.f;
static const float capacityConstant = 0.3f;
static const float minGradient = 0.25f;
static const float dissolveConstant = 0.2f;
static const float depositionConstant = 1.f;

TerrainCell::TerrainCell()
{
	Velocity = Vector3::Zero;
	Water = 0.f;
	Soil = 0.f;
	Suspended = 0.f;
	NextSuspended = 0.f;
	Gradient = 0.f;
	TotalFlux = 0.f;
}

float TerrainCell::Volume()
{
	return 0.0f;
}

float TerrainCell::DeltaHeight(TerrainCell & adjacent)
{
	return (Height + Water) - (adjacent.Height + adjacent.Water);
}

void TerrainCell::UpdateGradient()
{
	Gradient = 0.f;
	int sampleCount = 0;
	for (int i = 0; i < 4; i++) {
		if (Adjacent[i] != nullptr) {
			/*if (Velocity.x > 0 && i == 0
				|| Velocity.x < 0 && i == 2
				|| Velocity.z > 0 && i == 1
				|| Velocity.z < 0 && i == 3) {
			}*/
			Gradient += std::abs(std::tan(Height - Adjacent[i]->Height));
			sampleCount++;
		}
	}
	
	Gradient = (sampleCount == 0 ? 0 : Gradient / sampleCount);
}

void TerrainCell::ApplyDeltas()
{
	Suspended = NextSuspended * transferConstant;
	NextSuspended = 0;
}

void TerrainCell::Rain(float t)
{
	
	if (Utility::Chance(rainProbability)) {
		Water += rainQty * t;
	}
}

void TerrainCell::Evaporate(float t)
{
	//Water = std::max(0.f, Water - evaporationConstant*t);
	
	Water -= Water * evaporationConstant *t;
}

void TerrainCell::UpdateFlux(float t)
{
	
	// calculate initial flux
	float totalFlux = 0.f;
	float length = 1;
	float crossSectionalArea = length * length; //(Water + adjacent.Water) / 2.f;
	//Vector2(1.f, (Height + Water - (adjacent.Height + adjacent.Water)) / 2.f).Length();
	float fluxFactor = (t * crossSectionalArea * g) / length;
	for (int i = 0; i < 4; i++) {
		if (Adjacent[i] != nullptr) {
			TerrainCell& adjacent = *Adjacent[i];
			// energy loss
			OutFlux[i] *= damping;
			OutFlux[i] = std::max(0.f,
				OutFlux[i] + DeltaHeight(adjacent)* fluxFactor);
			totalFlux += OutFlux[i];
		}
		else {
			OutFlux[i] = 0.f;
		}
	}
	
	// scale down proportionate to water volume
	if (totalFlux > 0.f) {
		float scaleFactor = std::min(1.f, Water / (totalFlux * t));
		for (int i = 0; i < 4; i++) {
			OutFlux[i] *= scaleFactor;
		}
	}
}

void TerrainCell::UpdateWater(float t)
{
	// calculate total in and out flux
	float totalInFlux = 0.f;
	float totalOutFlux = 0.f;

	float left = 0.f;
	float right = 0.f;
	float top = 0.f;
	float bottom = 0.f;

	for (int i = 0; i < 4; i++) {
		if (Adjacent[i] != nullptr) {
			TerrainCell& adjacent = *Adjacent[i];

			int adjIndex = (i + 2) % 4;
			totalInFlux += adjacent.OutFlux[adjIndex];
			totalOutFlux += OutFlux[i];

			switch (i) {
			case 0:
				right += OutFlux[i];
				left += adjacent.OutFlux[adjIndex];
				break;
			case 2:
				left += OutFlux[i];
				right += adjacent.OutFlux[adjIndex];
				break;
			case 1:
				top += OutFlux[i];
				bottom += adjacent.OutFlux[adjIndex];
				break;
			case 3:
				bottom += OutFlux[i];
				top += adjacent.OutFlux[adjIndex];
				break;
			}
		}
	}
	TotalFlux = totalInFlux + totalOutFlux;
	float deltaVolume = t * (totalInFlux - totalOutFlux);
	float finalWater = Water + deltaVolume;
	float avgWater = (finalWater + Water) / 2.f;
	// update velocities
	Velocity = avgWater != 0 ? Vector3(((right - left)/2.f)/viscocity, 0.f, ((top - bottom)/2.f)/viscocity) : Vector3::Zero;

	Water = finalWater;
}

void TerrainCell::Erode(float t)
{
	
	float capacity =  Water * capacityConstant * std::max(0.f, (Utility::Sigmoid(Velocity.Length(),
		2.f,0.f,1.f)-1.f));
	// Destroy lone spires ----------
	/*float average = 0.0;
	for (int i = 0; i < 4; i++) {
		if (Adjacent[i] != nullptr) {
			if (Adjacent[i]->Height > Height) {
				break;
			}
			if (i == 3) {
				Suspended += Height - Adjacent[i]->Height;
				Height = Adjacent[i]->Height;
				
			}
		}
		else {
			break;
		}
	}*/
	//-------------------
	float difference = capacity - Suspended;
	if (capacity > Suspended) {
		Suspended += difference * dissolveConstant;
		Height -= difference * dissolveConstant;
		//float erodeQty = dissolveConstant * difference;
		////for (int i = 0; i < 4; i++) {
		////	if (Adjacent[i] != nullptr) {
		////		TerrainCell& adjacent = *Adjacent[i];
		////		adjacent.Height -= erodeQty * 0.2;
		////	}
		////}
		
		////// Erode
		
		//Suspended += dissolveConstant * difference;
		//float erodedSoil = std::min(Soil, difference / soilResistance);
		//difference -= erodedSoil * soilResistance;
		//float erodedStone = std::max(0.f, difference / stoneResistance);

		//Height -= (erodedSoil + erodedStone);
		//Soil -= erodedSoil;

		//Suspended += (erodedSoil + erodedStone);
		
	} 
	else if (Suspended > capacity) {
		// Deposit
		Height += -(depositionConstant * difference);
		Suspended -= -(depositionConstant * difference);
	}
}

void TerrainCell::Deposit(float t)
{
	float capacity = capacityConstant * std::max(0.f, (Utility::Sigmoid(Velocity.Length(),
		2.f, 0.f, 0.1f) - 1.f));

	//-------------------
	float difference = Suspended - capacity;

	if (Suspended > capacity) {
		// Deposit
		Height += (depositionConstant * difference);
		Suspended -= (depositionConstant * difference);
	}
}

void TerrainCell::Transport(float t)
{
	float angle = std::atan(std::abs(Velocity.z / Velocity.x));
	int adjZ = Utility::sign(Velocity.z);
	float zTransfer = std::sin(angle) * Suspended;

	int adjX = Utility::sign(Velocity.x);
	float xTransfer = std::cos(angle) * Suspended;
	Suspended -= (Suspended);
	if (adjZ != 0) {
		int zIndex = adjZ + 2;
		if (Adjacent[zIndex] != nullptr) Adjacent[zIndex]->NextSuspended += zTransfer;
	}
	if (adjX != 0) {
		int xIndex = -adjX + 1;
		if (Adjacent[xIndex] != nullptr) Adjacent[xIndex]->NextSuspended += xTransfer;
	}
}


