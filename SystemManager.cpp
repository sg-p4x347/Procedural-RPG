#include "pch.h"
#include "SystemManager.h"
#include "WorldSystem.h"
#include "TerrainSystem.h"
#include "PlayerSystem.h"
#include "RenderSystem.h"
#include "MovementSystem.h"
#include "GuiSystem.h"
//#include "InfrastructureSystem.h"

SystemManager::SystemManager(
	HWND window, int width, int height
) {
	//----------------------------------------------------------------
	// Construct the entity manager
	m_entityManager = std::shared_ptr<EntityManager>(new EntityManager());
	//----------------------------------------------------------------
	// Construct the core systems 

	
	AddSystem(std::shared_ptr<System>(new GuiSystem( 1)));
	AddSystem(std::shared_ptr<System>(new RenderSystem(m_entityManager, vector<string>{"VBO"}, 1, window, width, height, dynamic_pointer_cast<GuiSystem>(m_systems["Gui"]))));
	
	//AddSystem(std::shared_ptr<System>(new InfrastructureSystem(m_entityManager, vector<string>{"Infrastructure"}, 0)));

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

void SystemManager::LoadWorld(Filesystem::path worldDir)
{
	Filesystem::path systemsDir = worldDir / "System";
	Filesystem::create_directories(systemsDir);

	Filesystem::path assetsDir = worldDir / "Assets";
	Filesystem::create_directories(assetsDir);
	AssetManager::Get()->SetProceduralAssetDir(assetsDir);

	m_entityManager->SetDirectory(worldDir / "Component");

	auto renderSystem = dynamic_pointer_cast<RenderSystem>(m_systems["Render"]);
	renderSystem->SetEntityManager(m_entityManager);
	AddSystem(std::shared_ptr<System>(new TerrainSystem(m_entityManager, vector<string>{ "Terrain", "Position", "VBO" }, 1, 128, systemsDir)));
	AddSystem(std::shared_ptr<System>(new PlayerSystem(m_entityManager, vector<string>{ "Player", "Position" }, 1)));
	AddSystem(std::shared_ptr<System>(new MovementSystem(m_entityManager, vector<string>{"Movement"}, 1,renderSystem )));
	
	Initialize();
}

void SystemManager::CloseWorld()
{
	m_entityManager->UnInitialize();
}

void SystemManager::Save()
{
	for (auto & system : m_systems) system.second->Save();
}

void SystemManager::RunAll()
{
	for (auto & pair : m_systems) {
		pair.second->Run();
	}
}

void SystemManager::AddSystem(shared_ptr<System> system)
{
	m_systems.insert(std::pair<string, std::shared_ptr<System>>(system->Name(),system));
}
