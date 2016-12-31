#pragma once
#include "PositionComponent.h"
#include "PhysicsComponent.h"

using namespace std;

class EntityManager
{
public:
	EntityManager(string worldName);
	void RemoveEntity(unsigned int id);
	~EntityManager();

private:
	// store the entity components
	vector<PositionComponent> m_position; // x y z
	vector<PhysicsComponent> m_physics; // velocity and acceleration
};

