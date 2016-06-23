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
		XMMATRIX getViewMatrix();
		void getBaseViewMatrix(XMMATRIX&);
		// get user input and move accordingly
		void update(float elapsed, DirectX::Mouse::State mouse, DirectX::Keyboard::State keyboard);
	private:
		XMMATRIX viewMatrix;
		XMMATRIX baseViewMatrix;
};

