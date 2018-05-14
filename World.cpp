#include "pch.h"
#include "World.h"
// Systems
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
#include "ItemSystem.h"
#include "PlantSystem.h"
#include "ResourceSystem.h"
// Resources
#include "IEventManager.h"
#include "AssetManager.h"
#include "SystemManager.h"
World::World(
	SystemManager & systemManager,
	Filesystem::path parentDir,
	string name
) : m_systemManager(systemManager), m_directory(parentDir / name), m_name(name), m_width(0)
{
	Load();
}

World::World(
	SystemManager & systemManager, 
	Filesystem::path parentDir, 
	string name, 
	int seed
) : World::World(systemManager,parentDir,name)
{
	Generate(seed);
}

World::~World()
{
	IEventManager::Invoke(EventTypes::Sound_StopMusic);
	AssetManager::Get()->CleanupProceduralAssets();
	

	//----------------------------------------------------------------
	// Save all systems
	m_systemManager.Save();
	//----------------------------------------------------------------
	// Deconstruct the entity manager
	m_entityManager->Save();
	m_entityManager.reset();
	//----------------------------------------------------------------
	// Update render targets
	m_systemManager.GetSystem<RenderSystem>("Render")->InitializeWorldRendering(nullptr);
	//----------------------------------------------------------------
	// Remove world systems
	m_systemManager.Remove<WorldSystem>();
}

int World::GetWidth()
{
	return m_width;
}


void World::PauseGame()
{
	DirectX::Mouse::Get().SetMode(DirectX::Mouse::Mode::MODE_ABSOLUTE);
	m_systemManager.GetSystem<GuiSystem>("Gui")->OpenMenu("game_paused");
	HaltWorldSystems();
	m_paused = true;
	IEventManager::Invoke(EventTypes::Sound_StopMusic);
}

void World::ResumeGame()
{
	
	RunWorldSystems();
	m_paused = false;
	IEventManager::Invoke(EventTypes::Sound_PlayMusic);
	srand(clock());
}

void World::TogglePause()
{
	if (m_paused) {
		ResumeGame();
	}
	else {
		PauseGame();
	}
}

void World::HaltWorldSystems()
{
	m_systemManager.Halt<WorldSystem>();
}

void World::RunWorldSystems()
{
	m_systemManager.Run<WorldSystem>();
}
void World::Generate(int seed)
{
	// seed the RNG
	srand(seed);
	m_entityManager->GenerateEntityRegions();
	m_systemManager.GetSystem<TerrainSystem>("Terrain")->Generate();
	m_systemManager.GetSystem<PlantSystem>("Plant")->Generate();
	m_systemManager.GetSystem<PlayerSystem>("Player")->CreatePlayer();
	
}

bool World::Load()
{

	IEventManager::NewVersion();
	m_systemManager.GetSystem<GuiSystem>("Gui")->BindHandlers();
	//----------------------------------------------------------------
	// World configuration
	JsonParser terrainMap = JsonParser(std::ifstream("config/continent.json"))["terrainMap"];
	m_width = terrainMap["width"].To<int>();
	//----------------------------------------------------------------
	// Filesystem dependencies
	Filesystem::path systemsDir = m_directory / "System";
	Filesystem::create_directories(systemsDir);

	Filesystem::path assetsDir = m_directory / "Assets";
	Filesystem::create_directories(assetsDir);
	AssetManager::Get()->SetProceduralAssetDir(assetsDir);

	Filesystem::path componentDir = m_directory / "Component";
	Filesystem::create_directory(componentDir);
	//----------------------------------------------------------------
	// Create a new entity manager
	m_entityManager.reset(new WorldEntityManager(componentDir, m_width,32));
	//----------------------------------------------------------------
	// Update render targets
	auto renderSystem = m_systemManager.GetSystem<RenderSystem>("Render");
	renderSystem->InitializeWorldRendering(m_entityManager.get());
	//----------------------------------------------------------------
	// Add world systems
	m_systemManager.AddSystem(std::shared_ptr<System>(new PlayerSystem(&m_systemManager, m_entityManager, vector<string>{ "Player", "Position" }, 1)));
	m_systemManager.AddSystem(std::shared_ptr<System>(new TerrainSystem(&m_systemManager, m_entityManager, vector<string>{ "Terrain", "Position", "VBO" }, 1, m_width,64, systemsDir)));
	
	m_systemManager.AddSystem(std::shared_ptr<System>(new MovementSystem(m_entityManager, vector<string>{"Movement"}, 1, renderSystem)));
	m_systemManager.AddSystem(std::shared_ptr<System>(new CollisionSystem(&m_systemManager, m_entityManager, vector<string>{"Movement", "Position", "Collision"}, 1)));

	m_systemManager.AddSystem(std::shared_ptr<System>(new ActionSystem(&m_systemManager, m_entityManager, vector<string>{"Action", "Position"}, 10)));
	m_systemManager.AddSystem(std::shared_ptr<System>(new PlantSystem(&m_systemManager, m_entityManager, vector<string>(), 0)));
	m_systemManager.AddSystem(std::shared_ptr<System>(new ItemSystem(m_entityManager, vector<string>{"Inventory", "Item"}, 0)));
	m_systemManager.AddSystem(std::shared_ptr<System>(new BuildingSystem(m_entityManager, vector<string>{"Building"}, 0)));
	m_systemManager.AddSystem(std::shared_ptr<System>(new ResourceSystem(m_entityManager, vector<string>(), 0)));

	m_systemManager.Initialize();
	return true;
}

void World::SetMousePos(Vector2 pos)
{
	m_systemManager.GetSystem<PlayerSystem>("Player")->SetMousePos(pos);
}
