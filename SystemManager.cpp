#include "pch.h"
#include "SystemManager.h"

#include "TerrainSystem.h"
#include "PlayerSystem.h"
#include "RenderSystem.h"
#include "MovementSystem.h"
#include "InfrastructureSystem.h"

SystemManager::SystemManager(
	Filesystem::path directory,
	std::shared_ptr<EntityManager> entityManager,
	HWND window, int width, int height,
	std::shared_ptr<DirectX::Mouse> mouse,
	std::shared_ptr<DirectX::Keyboard> keyboard
) : m_entityManager(entityManager)
{
	 
	Filesystem::create_directories(directory);
	//----------------------------------------------------------------
	// Construct the systems 

	AddSystem(std::shared_ptr<System>(new TerrainSystem(m_entityManager, vector<string>{ "Terrain","Position","VBO" }, 120, 1024, directory)));
	AddSystem(std::shared_ptr<System>(new PlayerSystem(m_entityManager, vector<string>{ "Player","Position" }, 1,mouse,keyboard)));
	AddSystem(std::shared_ptr<System>(new RenderSystem(m_entityManager, vector<string>{"VBO"}, 1, window, width, height)));
	AddSystem(std::shared_ptr<System>(new MovementSystem(m_entityManager, vector<string>{"Movement"}, 1)));
	AddSystem(std::shared_ptr<System>(new MovementSystem(m_entityManager, vector<string>{"Infrastructure"}, 0)));

	//----------------------------------------------------------------
	// Initialize the systems

	//GetSystem<InfrastructureSystem>("Infrastructure")->SetTerrainSystem(GetSystem<TerrainSystem>("Terrain"));
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

void SystemManager::Initialize()
{
	for (auto & system : m_systems) system.second->Initialize();
}

void SystemManager::Save()
{
	for (auto & system : m_systems) system.second->Save();
}

void SystemManager::AddSystem(shared_ptr<System> system)
{
	m_systems.insert(std::pair<string, std::shared_ptr<System>>(system->Name(),system));
}
