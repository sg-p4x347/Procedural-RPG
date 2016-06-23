#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class Entity {
	public:
		// constructors
		Entity();
		Entity(XMFLOAT3 POSITION, float MASS);
		Entity(XMFLOAT3 POSITION, XMFLOAT3 VELOCITY, XMFLOAT3 ACCELERATION, float MASS);
		~Entity();
		// getters
		XMFLOAT3 getPosition();
		XMFLOAT3 getVelocity();
		XMFLOAT3 getAcceleration();
		float getMass();
		// setters
		void setPosition(XMFLOAT3);
		void setRotation(XMFLOAT3);
		void setVelocity(XMFLOAT3);
		void setAcceleration(XMFLOAT3);
		void setMass(float);
		// update
		void updatePhysics(float timeStep);
	protected:
		// movement properties
		XMFLOAT3 position = XMFLOAT3(0, 0, 0);
		XMFLOAT3 velocity = XMFLOAT3(0, 0, 0);
		XMFLOAT3 acceleration = XMFLOAT3(0, -9.8, 0);
		// angular properties
		XMFLOAT3 rotation = XMFLOAT3(0, 0, 0); // (yaw,pitch,roll)
		XMFLOAT3 angularVelocity = XMFLOAT3(0, 0, 0);
		XMFLOAT3 angularAcceleration = XMFLOAT3(0, 0, 0);
		// physical properties
		float mass = 1;
		// collistion model
		// display model
};

