#pragma once
#include "Component.h"
#include "Movement.h"

using namespace DirectX;

struct Movement : Component
{
public:
	Movement();
	Vector3 Velocity;
	Vector3 Acceleration;
	Vector3 AngularVelocity;
	Vector3 AngularAcceleration;
};

