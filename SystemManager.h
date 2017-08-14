#pragma once
#include "System.h"
#include "EntityManager.h"
#include "RenderSystem.h"

class SystemManager
{
public:
	SystemManager(
		string directory, 
		Microsoft::WRL::ComPtr<ID3D11Device> device, 
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		std::shared_ptr<DirectX::CommonStates> states,
		DirectX::Mouse::State mouse,
		DirectX::Keyboard::State keyboard
	);
	~SystemManager();
	// Updates systems according to their update period
	void Tick(double & elapsed);
private:
	//----------------------------------------------------------------
	// DirectX
	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
	//----------------------------------------------------------------
	// Systems
	vector<unique_ptr<System>> m_systems;
	//----------------------------------------------------------------
	// Entities
	shared_ptr<EntityManager> m_entityManager;


};

