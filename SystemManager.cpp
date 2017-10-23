#include "pch.h"
#include "SystemManager.h"

#include "TerrainSystem.h"
//#include "RenderSystem.h"
//#include "MovementSystem.h"
//#include "InputSystem.h"

SystemManager::SystemManager(
	Filesystem::path directory,
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
	std::shared_ptr<DirectX::CommonStates> states,
	std::shared_ptr<DirectX::Mouse> mouse,
	std::shared_ptr<DirectX::Keyboard> keyboard
) : m_device(device), m_context(context)
{
	//----------------------------------------------------------------
	// initialize the entity manager
	
	m_entityManager = shared_ptr<EntityManager>(new EntityManager(directory));
	//----------------------------------------------------------------
	// initialize the systems 
	Filesystem::path systemDir = directory / "System";
	Filesystem::create_directories(systemDir);
	// Terrain
	AddSystem(std::shared_ptr<System>(new TerrainSystem(m_entityManager, vector<string>({ "Player","Position" }), 120, 64, systemDir)));

	//// Render
	//m_systems.push_back(std::make_unique<System>(
	//	new RenderSystem(m_entityManager, vector<string>({ "Position" }),1,device,context,states))
	//);
	// Input
	/*m_systems.push_back(std::make_unique<System>(
		new InputSystem(m_entityManager, vector<string>({ "Input","Movement" }), 1, mouse,keyboard))
	);*/
}

SystemManager::~SystemManager()
{
}

void SystemManager::Tick(double & elapsed)
{
	for (auto & systemEntry : m_systems) {
		systemEntry.second->Tick(elapsed);
	}
}

void SystemManager::AddSystem(shared_ptr<System> system)
{
	m_systems.insert(std::pair<string, std::shared_ptr<System>>(system->Name(),system));
}
