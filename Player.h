#pragma once
#include "Component.h"
#include <DirectXMath.h>
using namespace DirectX;
namespace Components {
	class Player :
		public Component {
	public:
		Player();


		// Inherited via Component
		virtual string GetName() override;

	};
}
