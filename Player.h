#pragma once
#include "NPC.h"
#include <DirectXMath.h>
using namespace DirectX;

__declspec(align(16)) class Player : public NPC {
	public:
		Player();
		Player(XMFLOAT3 POSITION, float MASS);
		~Player();
		void* operator new(size_t i)
		{
			return _mm_malloc(i, 16);
		}

		void operator delete(void* p)
		{
			_mm_free(p);
		}
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

