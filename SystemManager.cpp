#include "pch.h"
#include <ctime>
#include "SystemManager.h"
#include "WorldSystem.h"
#include "TerrainSystem.h"
#include "PlayerSystem.h"
#include "RenderSystem.h"
#include "MovementSystem.h"
#include "GuiSystem.h"
#include "ActionSystem.h"
#include "CollisionSystem.h"
#include "IEventManager.h"
#include "SoundSystem.h"
#include "BuildingSystem.h"
//#include "InfrastructureSystem.h"

SystemManager::SystemManager(
	HWND window, int width, int height
) {
	//----------------------------------------------------------------
	// Construct the core systems 

	
	AddSystem(std::shared_ptr<System>(new GuiSystem( 1)));
	AddSystem(std::shared_ptr<System>(new RenderSystem(this,1, window, width, height, dynamic_pointer_cast<GuiSystem>(m_systems["Gui"]))));
	//AddSystem(std::shared_ptr<System>(new SoundSystem()));
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
	//----------------------------------------------------------------
	// Initialize all systems
	for (auto & system : m_systems) system.second->Initialize();
}

void SystemManager::LoadWorld(Filesystem::path worldDir)
{
	
	IEventManager::NewVersion();
	GetSystem<GuiSystem>("Gui")->BindHandlers();
	//----------------------------------------------------------------
	// Filesystem dependencies
	Filesystem::path systemsDir = worldDir / "System";
	Filesystem::create_directories(systemsDir);

	Filesystem::path assetsDir = worldDir / "Assets";
	Filesystem::create_directories(assetsDir);
	AssetManager::Get()->SetProceduralAssetDir(assetsDir);

	Filesystem::path componentDir = worldDir / "Component";
	Filesystem::create_directory(componentDir);
	//----------------------------------------------------------------
	// Create a new entity manager
	m_entityManager.reset(new WorldEntityManager(componentDir));
	//----------------------------------------------------------------
	// Update render targets
	auto renderSystem = GetSystem<RenderSystem>("Render");
	renderSystem->InitializeWorldRendering(m_entityManager.get());
	//----------------------------------------------------------------
	// Add world systems
	AddSystem(std::shared_ptr<System>(new TerrainSystem(this,m_entityManager, vector<string>{ "Terrain", "Position", "VBO" }, 1, 64, systemsDir)));
	AddSystem(std::shared_ptr<System>(new PlayerSystem(this,m_entityManager, vector<string>{ "Player", "Position" }, 1)));
	AddSystem(std::shared_ptr<System>(new MovementSystem(m_entityManager, vector<string>{"Movement"}, 1, renderSystem)));
	AddSystem(std::shared_ptr<System>(new ActionSystem(this, m_entityManager, vector<string>{"Action", "Position"}, 10)));
	AddSystem(std::shared_ptr<System>(new CollisionSystem(this, m_entityManager, vector<string>{"Movement", "Position", "Collision"}, 1)));
	AddSystem(std::shared_ptr<System>(new BuildingSystem(m_entityManager, vector<string>{"Building"}, 0)));
}

void SystemManager::CloseWorld()
{
	//----------------------------------------------------------------
	// Save all systems
	Save();
	//----------------------------------------------------------------
	// Deconstruct the entity manager
	if (m_entityManager) {
		m_entityManager->Save();
		m_entityManager.reset();
	}
	//----------------------------------------------------------------
	// Update render targets
	auto renderSystem = GetSystem<RenderSystem>("Render");
	renderSystem->InitializeWorldRendering(nullptr);
	//----------------------------------------------------------------
	// Remove world systems
	Remove<WorldSystem>();
}

void SystemManager::Save()
{
	for (auto & system : m_systems) system.second->Save();
}

void SystemManager::AddSystem(shared_ptr<System> system)
{
	m_systems.insert(std::pair<string, std::shared_ptr<System>>(system->Name(),system));
}
