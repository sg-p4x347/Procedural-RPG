#pragma once
#include "Component.h"
namespace Components {
	template <typename VertexType>
	class VBO :
		public Component
	{
	public:
		VBO() : LOD(-1), LODchanged(false), Effect("default")
		{

		}
		void CreateBuffers(Microsoft::WRL::ComPtr<ID3D11Device> device)
		{
			CreateVB(device);
			CreateIB(device);
		}

		// Vertex Buffer
		Microsoft::WRL::ComPtr<ID3D11Buffer> VB;
		vector<VertexType> Vertices;
		// Index Buffer
		Microsoft::WRL::ComPtr<ID3D11Buffer> IB;
		vector<uint16_t> Indices;
		// Effect to be rendered with
		string Effect;

		// Level Of Detail
		int LOD;
		bool LODchanged;
		// Inherited via Component
		void Export(std::ostream & ofs)
		{
			Serialize(Effect, ofs);
		}
		void Import(std::istream & ifs)
		{
			DeSerialize(Effect, ifs);
		}
		
	protected:
		void CreateVB(Microsoft::WRL::ComPtr<ID3D11Device> device)
		{
			D3D11_BUFFER_DESC desc = { 0 };
			desc.ByteWidth = (UINT)sizeof(VertexType)*Vertices.size();
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA initData = { 0 };
			initData.pSysMem = Vertices.data();

			DX::ThrowIfFailed(device->CreateBuffer(&desc, &initData,
				VB.ReleaseAndGetAddressOf()));
		}
		void CreateIB(Microsoft::WRL::ComPtr<ID3D11Device> device)
		{
			D3D11_BUFFER_DESC desc = { 0 };
			desc.ByteWidth = (UINT)sizeof(uint16_t)*Indices.size();
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			D3D11_SUBRESOURCE_DATA initData = { 0 };
			initData.pSysMem = Indices.data();

			DX::ThrowIfFailed(device->CreateBuffer(&desc, &initData,
				IB.ReleaseAndGetAddressOf()));
		}

		// Inherited via Component
		virtual string GetName() override
		{
			return "VBO";
		}
	};
}

