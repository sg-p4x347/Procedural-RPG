#pragma once
#include "System.h"
#include "VBO.h"
class RenderSystem :
	public System
{
public:
	RenderSystem(
		shared_ptr<EntityManager> & entityManager, 
		vector<string> & components, 
		unsigned short updatePeriod,
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		std::shared_ptr<DirectX::CommonStates> states
	);
	~RenderSystem();
private:
	// Inherited via System
	virtual void Update() override;
	virtual void SyncEntities() override;
	shared_ptr<Entity> m_player;
	vector<shared_ptr<Entity>> m_regions;
	// DirectX
	Microsoft::WRL::ComPtr<ID3D11Device>		m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
	std::shared_ptr<DirectX::CommonStates>		m_states;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
	std::unique_ptr<DirectX::DGSLEffect>		m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>	m_batch;

	DirectX::XMMATRIX GetViewMatrix();
	void RenderVBO(shared_ptr<Components::VBO> vbo, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, PrimitiveBatch<DirectX::VertexPositionColor> * batch);
	

	// Inherited via System
	virtual string Name() override;

};

