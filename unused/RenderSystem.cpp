#include "pch.h"
#include "RenderSystem.h"
#include "Position.h"

RenderSystem::RenderSystem(
	shared_ptr<EntityManager>& entityManager, 
	vector<string>& components, 
	unsigned short updatePeriod, 
	Microsoft::WRL::ComPtr<ID3D11Device> device, 
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
	std::shared_ptr<DirectX::CommonStates> states
) : System(entityManager,components, updatePeriod), m_device(device),m_context(context),m_states(states)
{

}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::Update()
{
	//https://stackoverflow.com/questions/17655442/how-can-i-repeat-my-texture-in-dx

	//TODO: Add your rendering code here.
	m_effect->Apply(m_context.Get());

	// camera
	m_effect->SetView(GetViewMatrix());

	// Render regions
	auto sampler = m_states->LinearWrap();

	m_context->PSSetSamplers(0, 1, &sampler);
	m_context->RSSetState(m_states->CullCounterClockwise());
	m_context->IASetInputLayout(m_inputLayout.Get());

	for (shared_ptr<Entity> entity : m_entities) {
		if (!m_regions->data[i].IsNull()) {
			m_regions->data[i].Render(context, m_batch.get());
		}
	}
}

void RenderSystem::SyncEntities()
{
	m_entities = m_entityManager->EntitiesContaining("Terrain", m_componentMask);
}

DirectX::XMMATRIX RenderSystem::GetViewMatrix()
{
	shared_ptr<Components::Position> position = m_player->GetComponent<Components::Position>("Position");
	float y = sinf(position->Rot.y);
	float r = cosf(position->Rot.y);
	float z = r*cosf(position->Rot.x);
	float x = r*sinf(position->Rot.x);

	XMVECTOR lookAt = position->Pos + SimpleMath::Vector3(x, y, z);

	XMMATRIX view = XMMatrixLookAtRH(position->Pos, lookAt, SimpleMath::Vector3(0.f, 1.f, 0.f));
	return view;
}

void RenderSystem::RenderVBO(shared_ptr<Components::VBO> vbo, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, PrimitiveBatch<DirectX::VertexPositionColor>* batch)
{
	// Set vertex buffer stride and offset.
	UINT stride = sizeof(VertexPositionNormalTangentColorTexture);
	UINT offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, vbo->VB.GetAddressOf(), &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(vbo->IB.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw
	deviceContext->DrawIndexed(vbo->Indices.size(), 0, 0);
}

string RenderSystem::Name()
{
	return "Render";
}
