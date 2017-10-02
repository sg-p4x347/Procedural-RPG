#include "pch.h"
#include "VBO.h"

namespace Component {
	VBO::VBO()
	{
	}

	VBO::VBO(const unsigned int & id) : Component::Component(id)
	{

	}


	VBO::~VBO()
	{
	}
	void VBO::CreateBuffers(shared_ptr<ID3D11Device> device)
	{
		CreateVB(device);
		CreateIB(device);
	}
	shared_ptr<Component> VBO::GetComponent(const unsigned int & id)
	{
		// Query in-memory list
		for (VBO & vbo : GetComponents()) {
			if (vbo.ID == id) {
				return std::make_shared<Component>(&vbo);
			}
		}
		// Not found
		return std::make_shared<Component>(nullptr);
	}
	void VBO::SaveAll(string directory)
	{
	}
	void VBO::Attach(shared_ptr<Component> component)
	{
		GetComponents().push_back(*dynamic_cast<VBO*>(component.get()));
	}
	string VBO::GetName()
	{
		return "VBO";
	}
	shared_ptr<Component> VBO::Create(std::ifstream & ifs)
	{
		return shared_ptr<Component>(new VBO());
	}
	void VBO::CreateVB(shared_ptr<ID3D11Device> device)
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
	void VBO::CreateIB(shared_ptr<ID3D11Device> device)
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
	vector<VBO>& VBO::GetComponents()
	{
		static vector<VBO> components;
		return components;
	}
}
