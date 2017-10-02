#pragma once
#include "Component.h"
#include <DirectXMath.h>
using namespace DirectX;
namespace Component {
	class Player :
		public Component {
	public:
		Player();
		
		// Direct X pipeline
		XMMATRIX getViewMatrix();
		void getBaseViewMatrix(XMMATRIX&);
		// get user input and move accordingly
		void update(float elapsed, DirectX::Mouse::State mouse, DirectX::Keyboard::State keyboard);
	private:
		XMMATRIX viewMatrix;
		XMMATRIX baseViewMatrix;
	};
}
