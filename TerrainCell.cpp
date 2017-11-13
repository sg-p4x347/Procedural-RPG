#include "pch.h"
#include "TerrainCell.h"
#include "Utility.h"

TerrainCell::TerrainCell()
{
	Velocity = Vector3::Zero;
	Water = 0.f;
	Soil = 0.f;
	Dissolved = 0.f;
}

void TerrainCell::operator+=(TerrainCell & deltas)
{
	Velocity += deltas.Velocity;
	Water += deltas.Water;
	Soil += deltas.Soil;
	Dissolved += deltas.Dissolved;
}

void TerrainCell::ApplyDeltas(TerrainCell & deltas)
{
	*this += deltas;
	deltas.Velocity = Vector3::Zero;
	deltas.Water = 0.f;
	deltas.Soil = 0.f;
	deltas.Dissolved = 0.f;
}

void TerrainCell::Erode()
{
	static const float maxSolution = 0.3;
	static const float stoneResistance = 2;
	static const float maxErosion = 1.f;
	if (Water > 0.01f && Volume() > .01f) {
		float deltaSolution = maxSolution - Dissolved / Volume();
		float erosionDepth = std::min(maxErosion,Utility::Inverse(Velocity.Length(), deltaSolution, 0, 1) * Volume());
		if ((erosionDepth + Dissolved) / Volume() > maxSolution) {
			int nono = 0;
		}
		// erode the dirt
		float erodedSoil = std::min(erosionDepth, Soil);
		Soil -= erodedSoil;
		Dissolved += erodedSoil;

		erosionDepth -= erodedSoil;
		// erode the stone
		float erodedStone = erosionDepth / stoneResistance;
		Dissolved += erodedStone;

		// update Height
		Height -= (erodedSoil + erodedStone);

	}

}

void TerrainCell::Deposit()
{
	static const float depositionRate = 0.1f;
	if (Dissolved > 0.f) {
		float deposition = std::min(depositionRate, Dissolved);
		Soil += deposition;
		Dissolved -= deposition;
		Height += deposition;
	}
}


