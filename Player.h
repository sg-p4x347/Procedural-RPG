#pragma once
#include "NPC.h"
#include <DirectXMath.h>
using namespace DirectX;

class Player : public NPC {
	public:
		Player();
		Player(XMFLOAT3 POSITION, float MASS);
		~Player();

		// Direct X pipeline
		void render();
		void renderBaseViewMatrix();
		void getViewMatrix(XMMATRIX&);
		void getBaseViewMatrix(XMMATRIX&);
		// get user input and move accordingly
		void update();
	private:
		XMMATRIX viewMatrix;
		XMMATRIX baseViewMatrix;
};

