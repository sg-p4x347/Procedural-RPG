#include "pch.h"
#include "Droplet.h"
#include "Utility.h"

static const float g_inertia = 0.4f;
static const float g_minSlope = 0.1f;
static const float g_capacity = 16.f;
static const float g_deposition = 0.15f;
static const float g_erosion = 0.01f;
static const float g_gravity = 3.f;
static const float g_evaporation = 0.01f;
static const int g_maxPath = 10000;
static const float g_minRadius = 1.f;
static const float g_maxRadius = 1.f;
static const float g_maxVelocity = 100.f;
static const float g_damping = 0.9f;
static const float g_minHeight = -10.f;
static const float g_minCarryCapacity = 0.01f;
const float Droplet::Radius = 1.f;
Droplet::Droplet()
{
	
}

void Droplet::Reset(int width)
{
	//Position = Vector2(width / 2, width / 2);
	Position = Vector2(Utility::randWithin(1, width - 1), Utility::randWithin(1, width - 1));
}

void Droplet::Reset(HeightMap & map)
{
	/*if (Position.x >= 0 && Position.y >= 0 && Position.x <= map.width && Position.y <= map.width) {
		changeMap.map[Position.x][Position.y] += Sediment;
	}*/
	Reset(map.width);
	Direction = Vector2::Zero;
	Water = 2.f;
	Sediment = 0.f;
	Velocity = 0.f;
	//PathLength = 0;
	//Radius = Utility::randWithin(g_minRadius, g_maxRadius);
}

void Droplet::Update(HeightMap & map)
{
	if ( map.map[(int)Position.x][(int)Position.y] < g_minHeight || Water <= 0.01f || Position.x <= 1 || Position.y <= 1 || Position.x >= map.width - 1 || Position.y >= map.width - 1) {
		Reset(map);
		return;
	}
	Vector2 gradient = map.GradientDirection(Position.x, Position.y);
	// Direction
	Direction = Direction * g_inertia - gradient*(1 - g_inertia);
	
	if (Direction.Length() == 0.f) {
		Direction = Vector2(Utility::randWithin(-1.f, 1.f), Utility::randWithin(-1.f, 1.f));
	}
	Direction.Normalize();
	// Position
	Vector2 oldPosition = Position;
	Position += Direction;
	
	// Heights
	float oldHeight = map.Height(oldPosition.x, oldPosition.y);
	float dh = map.Height(Position.x, Position.y) - oldHeight;
	// Velocity
	Velocity = std::min(g_maxVelocity,std::sqrt(std::abs(Velocity * Velocity + dh * g_gravity))) * g_damping;
	// Water
	float oldWater = Water;
	Water = Water * (1 - g_evaporation);

	// Erosion/Deposition
	float carryCapacity = std::max(-dh, g_minSlope) * Velocity * Water * g_capacity;
	if (carryCapacity < g_minCarryCapacity) {
		Reset(map);
		return;
	}
	if (Sediment < carryCapacity) {
		//=======
		// Erode
		//=======
		float maxErode = std::min((carryCapacity - Sediment)* g_erosion, -dh);

		// Calculate amount to erode from surrounding neighbors (Cone shaped pen)
		const int weightCount = (Radius + 1) * (Radius + 1) * 4;
		unique_ptr<float> weights = std::unique_ptr<float>(new float[weightCount]);
		int i = 0;
		for (int x = oldPosition.x - Radius; x < oldPosition.x + Radius; x++) {
			for (int z = oldPosition.y - Radius; z < oldPosition.y + Radius; z++) {
				if (x >= 0 && x <= map.width && z >= 0 && z <= map.width) {
					weights.get()[i] = std::max(0.f, Radius - (Vector2(x, z) - oldPosition).Length());
				}
				i++;
			}
		}
		Utility::Normalize(weights.get(),weightCount);
		// Erode
		i = 0;
		for (int x = oldPosition.x - Radius; x < oldPosition.x + Radius; x++) {
			for (int z = oldPosition.y - Radius; z < oldPosition.y + Radius; z++) {
				if (x >= 0 && x <= map.width && z >= 0 && z <= map.width) {
					float deltaVolume = -maxErode * weights.get()[i];
					map.map[x][z] += deltaVolume;
					i++;
				}
			}
		}
		Sediment += maxErode;
	}
	else if (Sediment > carryCapacity) {
		//=======
		// Deposit
		//=======
		float maxDeposit = (Sediment - carryCapacity) * g_deposition;

		// Calculate amount to erode from surrounding neighbors (Cone shaped pen)
		const int weightCount = (Radius + 1) * (Radius + 1) * 4;
		unique_ptr<float> weights = std::unique_ptr<float>(new float[weightCount]);
		int i = 0;
		for (int x = oldPosition.x - Radius; x < oldPosition.x + Radius; x++) {
			for (int z = oldPosition.y - Radius; z < oldPosition.y + Radius; z++) {
				if (x >= 0 && x <= map.width && z >= 0 && z <= map.width) {
					weights.get()[i] = (std::max(0.f, Radius - (Vector2(x, z) - oldPosition).Length())*(oldHeight-map.map[x][z]));
				
				}
				i++;
			}
		}
		Utility::Normalize(weights.get(),weightCount);
		// Deposit
		i = 0;
		for (int x = oldPosition.x - Radius; x < oldPosition.x + Radius; x++) {
			for (int z = oldPosition.y - Radius; z < oldPosition.y + Radius; z++) {
				if (x >= 0 && x <= map.width && z >= 0 && z <= map.width) {
					float deltaVolume = maxDeposit * weights.get()[i];
					map.map[x][z] += deltaVolume;
					i++;
				}
			}
		}
		Sediment -= maxDeposit;

	}

	//PathLength++;
}
