#pragma once
#include <directxmath.h>
#include "Component.h"
#include "Movement.h"

using namespace DirectX;

class Movement : Component
{
public:
	Movement();
private:
	XMFLOAT3 m_velocity;
	XMFLOAT3 m_acceleration;
	XMFLOAT3 m_angularVelocity;
	XMFLOAT3 m_angularAcceleration;
};

