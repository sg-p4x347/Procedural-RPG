#pragma once
#include <directxmath.h>
#include "PhysicsComponent.h"

using namespace DirectX;

struct PositionComponent
{
	PositionComponent(unsigned int id, XMFLOAT3 pos, XMFLOAT3 rot);
	~PositionComponent();
	unsigned int ID;
	XMFLOAT3 position;
	XMFLOAT3 rotation;
};

