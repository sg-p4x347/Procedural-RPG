#pragma once
#include "System.h"
#include "EntityManager.h"
//#include "RenderSystem.h"

class SystemManager
{
public:
	SystemManager(
		Filesystem::path directory,
		Microsoft::WRL::ComPtr<ID3D11Device> device, 
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		std::shared_ptr<DirectX::CommonStates> states,
		std::shared_ptr<DirectX::Mouse> mouse,
		std::shared_ptr<DirectX::Keyboard> keyboard
	);
	~SystemManager();
	// Updates systems according to their update period
	void Tick(double & elapsed);
	map<string, shared_ptr<System>> m_systems;
private:
	//----------------------------------------------------------------
	// DirectX
	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
	//----------------------------------------------------------------
	// Systems
	void AddSystem(shared_ptr<System> system);
	
	//----------------------------------------------------------------
	// Entities
	shared_ptr<EntityManager> m_entityManager;


};

