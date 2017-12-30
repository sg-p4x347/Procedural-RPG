#include "pch.h"
#include "ThermalCell.h"

const float ThermalCell::m_talusAngle = 2.f;
const float ThermalCell::m_g = 9.8f;
const float ThermalCell::m_dt = 0.1f;
ThermalCell::ThermalCell()
{
	for (int i = 0; i < 4; i++) {
		OutFlux[i] = 0.f;
	}
}

float ThermalCell::DeltaHeight(HeightMap & map, Map<ThermalCell>& terrain, int & x, int & z, int & adjX, int & adjZ)
{
	return map.map[x][z] - map.map[adjX][adjZ];
}

void ThermalCell::UpdateFlux(HeightMap & map, Map<ThermalCell> & terrain, int & x, int & z)
{

	// calculate initial flux
	float totalFlux = 0.f;
	
	static const float fluxFactor = m_dt * m_g;
	
	for (int i = 0; i < 4; i++) {
		int adjX;
		int adjZ;
		Adjacent(i, x, z, adjX, adjZ);
		if (terrain.Bounded(adjX, adjZ)) {
			float deltaHeight = DeltaHeight(map,terrain,x,z,adjX,adjZ);
			if (deltaHeight > m_talusAngle) {
				OutFlux[i] = std::max(0.f,
					OutFlux[i] + deltaHeight * fluxFactor);
				totalFlux += OutFlux[i];
			}
			else {
				OutFlux[i] = 0.f;
			}
		}
		else {
			OutFlux[i] = 0.f;
		}
		
	}

	// scale down proportionate to volume
	if (totalFlux > 0.f) {
		float scaleFactor = std::min(1.f, std::abs(map.map[x][z] / (totalFlux * m_dt)));
		for (int i = 0; i < 4; i++) {
			OutFlux[i] *= scaleFactor;
		}
	}
}

void ThermalCell::TransferSediment(HeightMap & map, Map<ThermalCell> & terrain, int & x, int & z)
{
	// calculate total in and out flux
	float totalInFlux = 0.f;
	float totalOutFlux = 0.f;

	for (int i = 0; i < 4; i++) {
		int adjX;
		int adjZ;
		Adjacent(i, x, z, adjX, adjZ);
		if (terrain.Bounded(adjX,adjZ)) {
			int adjIndex = (i + 2) % 4;
			totalInFlux += terrain.map[adjX][adjZ].OutFlux[adjIndex];
			totalOutFlux += OutFlux[i];
		}
	}
	float deltaVolume = m_dt * (totalInFlux - totalOutFlux);
	if (deltaVolume <= -10) {
		int test = 0;
	}
	map.map[x][z] += deltaVolume;
}
void ThermalCell::Adjacent(const int & i, const int & x, const int & z, int & adjX, int & adjZ)
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