#pragma once
#include "Component.h"
namespace Components {
	class VBO :
		public Component
	{
	public:
		VBO(const unsigned int & id);
		void CreateBuffers(Microsoft::WRL::ComPtr<ID3D11Device> device);

		// Vertex Buffer
		Microsoft::WRL::ComPtr<ID3D11Buffer> VB;
		vector<DirectX::VertexPositionNormalTangentColorTexture> Vertices;
		// Index Buffer
		Microsoft::WRL::ComPtr<ID3D11Buffer> IB;
		vector<unsigned int> Indices;
		// Effect to be rendered with
		string Effect;
		// Level Of Detail
		int LOD;
		bool LODchanged;
		// Inherited via Component
		virtual string GetName() override;
	protected:
		void CreateVB(Microsoft::WRL::ComPtr<ID3D11Device> device);
		void CreateIB(Microsoft::WRL::ComPtr<ID3D11Device> device);
	};
}

