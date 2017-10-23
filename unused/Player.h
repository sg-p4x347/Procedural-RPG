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
		// Inherited via Component
		virtual shared_ptr<Component> GetComponent(const unsigned int & id) override;
		virtual void SaveAll(string directory) override;
		virtual void Attach(shared_ptr<Component> & component) override;
		virtual string GetName() override;
		virtual shared_ptr<Component> Create(std::ifstream & ifs) override;
	protected:
		vector<Player> & GetComponents();
		XMMATRIX viewMatrix;
		XMMATRIX baseViewMatrix;

		
	};
}
