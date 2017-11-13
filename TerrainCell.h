#pragma once

using DirectX::SimpleMath::Vector3;
class TerrainCell
{
public:
	TerrainCell();
	float Volume() { return Water + Dissolved; }
	Vector3 Velocity;
	int X;
	int Z;
	float Water;
	float Soil;
	float Dissolved;
	float Height;


	void operator +=(TerrainCell & deltas);
	void ApplyDeltas(TerrainCell & deltas);
	void Erode();
	void Deposit();
};

