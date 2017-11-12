#include "pch.h"
#include "VBO.h"

namespace Components {

	VBO::VBO(const unsigned int & id) : Component::Component(id), LOD(-1), LODchanged(false)
	{

	}
	void VBO::CreateBuffers(Microsoft::WRL::ComPtr<ID3D11Device> device)
	{
		CreateVB(device);
		CreateIB(device);
	}


	string VBO::GetName()
	{
		return "VBO";
	}
	void VBO::CreateVB(Microsoft::WRL::ComPtr<ID3D11Device> device)
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = sizeof(DirectX::VertexPositionNormalTangentColorTexture)*Vertices.size();
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA initData = { 0 };
		initData.pSysMem = Vertices.data();

		DX::ThrowIfFailed(device->CreateBuffer(&desc, &initData,
			VB.ReleaseAndGetAddressOf()));
	}
	void VBO::CreateIB(Microsoft::WRL::ComPtr<ID3D11Device> device)
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = sizeof(unsigned int)*Indices.size();
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA initData = { 0 };
		initData.pSysMem = Indices.data();

		DX::ThrowIfFailed(device->CreateBuffer(&desc, &initData,
			IB.ReleaseAndGetAddressOf()));
	}

}
