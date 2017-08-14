#include "pch.h"
#include "SystemManager.h"
#include "MovementSystem.h"
#include "InputSystem.h"

SystemManager::SystemManager(
	string directory,
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
	std::shared_ptr<DirectX::CommonStates> states,
	DirectX::Mouse::State mouse,
	DirectX::Keyboard::State keyboard
) : m_device(device), m_context(context)
{
	// initialize the entity manager
	m_entityManager = make_shared<EntityManager>(EntityManager(directory));
	//----------------------------------------------------------------
	// initialize the systems 
	
	// Render
	m_systems.push_back(std::make_unique<System>(
		new RenderSystem(m_entityManager, vector<string>({ "Position" }),1,device,context,states))
	);
	// Input
	m_systems.push_back(std::make_unique<System>(
		new InputSystem(m_entityManager, vector<string>({ "Input","Movement" }), 1, mouse,keyboard))
	);
}

SystemManager::~SystemManager()
{
}

void SystemManager::Tick(double & elapsed)
{
	for (unique_ptr<System> & system : m_systems) {
		system->Tick(elapsed);
	}
}
