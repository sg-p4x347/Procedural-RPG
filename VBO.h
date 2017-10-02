#pragma once
#include "Component.h"
namespace Component {
	class VBO :
		public Component
	{
	public:
		VBO();
		VBO(const unsigned int & id);
		~VBO();

		void CreateBuffers(shared_ptr<ID3D11Device> device);

		// Vertex Buffer
		Microsoft::WRL::ComPtr<ID3D11Buffer> VB;
		vector<DirectX::VertexPositionNormalTangentColorTexture> Vertices;
		// Index Buffer
		Microsoft::WRL::ComPtr<ID3D11Buffer> IB;
		vector<unsigned int> Indices;

		// Inherited via Component
		virtual shared_ptr<Component> GetComponent(const unsigned int & id) override;
		virtual void SaveAll(string directory) override;
		virtual void Attach(shared_ptr<Component> component) override;
		virtual string GetName() override;
		virtual shared_ptr<Component> Create(std::ifstream & ifs) override;
	protected:
		void CreateVB(shared_ptr<ID3D11Device> device);
		void CreateIB(shared_ptr<ID3D11Device> device);
		vector<VBO> & GetComponents();
	};
}

