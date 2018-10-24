#include "pch.h"
#include "World.h"
#include "TaskManager.h"
// Systems
#include "WorldSystem.h"
#include "TerrainSystem.h"
#include "PlayerSystem.h"
#include "RenderSystem.h"
#include "MovementSystem.h"
#include "GuiSystem.h"
//#include "ActionSystem.h"
#include "CollisionSystem.h"
#include "IEventManager.h"
#include "SoundSystem.h"
//#include "BuildingSystem.h"
//#include "ItemSystem.h"
#include "PlantSystem.h"
//#include "ResourceSystem.h"
// Resources
#include "WorldDomain.h"
#include "IEventManager.h"
#include "AssetManager.h"
#include "SystemManager.h"
namespace world {
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
	) : World::World(systemManager, parentDir, name)
	{
		Generate(seed);
	}

	World::~World()
	{
		//----------------------------------------------------------------
		// Wait for all tasks to complete
		TaskManager::Get().WaitForAll();
		//----------------------------------------------------------------
		// Halt world systems
		m_systemManager.Halt<WorldSystem>();
		//----------------------------------------------------------------
		// Update render targets
		m_systemManager.GetSystem<RenderSystem>("Render")->InitializeWorldRendering(nullptr);

		
		
		//----------------------------------------------------------------
		// Save all systems
		m_systemManager.Save();
		//----------------------------------------------------------------
		// Deconstruct the entity manager
		m_entityManager.reset();
		//----------------------------------------------------------------
		// Remove world systems
		m_systemManager.Remove<WorldSystem>();
		//----------------------------------------------------------------
		// Clean up asset manager
		AssetManager::Get()->CleanupProceduralAssets();
		//----------------------------------------------------------------
		// Stop the music
		IEventManager::Invoke(EventTypes::Sound_StopMusic);
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
		auto position = m_entityManager->PlayerPos();
		IEventManager::Invoke(EventTypes::Movement_PlayerMoved, position.x, position.z);
	}
	void World::Generate(int seed)
	{
		// seed the RNG
		srand(seed);
		m_systemManager.GetSystem<PlayerSystem>("Player")->CreatePlayer();
		m_systemManager.GetSystem<TerrainSystem>("Terrain")->Generate();
		m_systemManager.GetSystem<PlantSystem>("Plant")->Generate();

	}

	bool World::Load()
	{

		IEventManager::NewVersion();
		m_systemManager.GetSystem<GuiSystem>("Gui")->BindHandlers();
		//----------------------------------------------------------------
		// World configuration
		JsonParser worldConfig = JsonParser(std::ifstream("config/world.json"));
		m_width = worldConfig["width"].To<int>();
		m_regionWidth = worldConfig["regionWidth"].To<int>();
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
		m_entityManager.reset(new WEM(componentDir, m_width, m_regionWidth,worldConfig["loadRange"].To<float>()));
		//----------------------------------------------------------------
		// Update render targets
		auto renderSystem = m_systemManager.GetSystem<RenderSystem>("Render");
		
		//----------------------------------------------------------------
		// Add world systems
		m_systemManager.AddSystem(std::shared_ptr<System>(new PlayerSystem(&m_systemManager, m_entityManager.get(), 1)));
		m_systemManager.AddSystem(std::shared_ptr<System>(new TerrainSystem(&m_systemManager, m_entityManager.get(), 1, m_width, m_regionWidth, systemsDir)));


		m_systemManager.AddSystem(std::shared_ptr<System>(new CollisionSystem(&m_systemManager, m_entityManager.get(), 1)));
		m_systemManager.AddSystem(std::shared_ptr<System>(new MovementSystem(m_entityManager.get(), 1, renderSystem)));
		

		//m_systemManager.AddSystem(std::shared_ptr<System>(new ActionSystem(&m_systemManager, m_entityManager.get(), 10)));
		m_systemManager.AddSystem(std::shared_ptr<System>(new PlantSystem(&m_systemManager, m_entityManager.get(), 0)));
		//m_systemManager.AddSystem(std::shared_ptr<System>(new ItemSystem(m_entityManager.get(), 0)));
		//m_systemManager.AddSystem(std::shared_ptr<System>(new BuildingSystem(m_entityManager.get(), 0)));
		//m_systemManager.AddSystem(std::shared_ptr<System>(new ResourceSystem(m_entityManager.get(), 0)));

		m_systemManager.Initialize();

		//----------------------------------------------------------------
		// Assets
		/*IEventManager::RegisterHandler(EventTypes::WEM_Resync, std::function<void(void)>([]() {
			AssetManager::Get()->CollectGarbage();
		}));*/
		return true;
	}

	void World::Render()
	{
		auto renderSystem = m_systemManager.GetSystem<RenderSystem>("Render");
		renderSystem->InitializeWorldRendering(m_entityManager.get());
	}

	void World::SetMousePos(Vector2 pos)
	{
		m_systemManager.GetSystem<PlayerSystem>("Player")->SetMousePos(pos);
	}
}