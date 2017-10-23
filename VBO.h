#pragma once
#include "Component.h"
namespace Components {
	class VBO :
		public Component
	{
	public:
		VBO(const unsigned int & id);
		VBO(const VBO & other);
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
		virtual string GetName() override;
		virtual shared_ptr<Component> Add(const unsigned int & id) override;
	protected:
		void CreateVB(shared_ptr<ID3D11Device> device);
		void CreateIB(shared_ptr<ID3D11Device> device);
		vector<VBO> & GetComponents();

	};
}

