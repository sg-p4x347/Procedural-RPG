#pragma once
#include "Entity.h"
class NPC : public Entity {
	public:
		NPC();
		NPC(XMFLOAT3 POSITION, float MASS);
		~NPC();
	protected:
		// humanoid stuff that Player also gets
		float movementSpeed = 50.0;
		void moveForward();
		void moveBackward();
		void stop();
	private:
		// AI stuff (does not get passed on to Player)
};

