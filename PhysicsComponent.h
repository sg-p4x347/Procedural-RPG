#pragma once
#include <directxmath.h>
#include "PhysicsComponent.h"

using namespace DirectX;

class PhysicsComponent
{
public:
	PhysicsComponent();
	~PhysicsComponent();
private:
	XMFLOAT3 m_velocity;
	XMFLOAT3 m_acceleration;
	XMFLOAT3 m_angularVelocity;
	XMFLOAT3 m_angularAcceleration;
};

