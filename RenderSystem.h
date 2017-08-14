#pragma once
#include "System.h"
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

	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
	std::shared_ptr<DirectX::CommonStates> m_states;
};

