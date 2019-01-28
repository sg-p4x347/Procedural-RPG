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
#include "GuiStyle.h"
//#include "ActionSystem.h"
#include "CollisionSystem.h"
#include "IEventManager.h"
#include "SoundSystem.h"
#include "BuildingSystem.h"
//#include "ItemSystem.h"
#include "PlantSystem.h"
#include "MovementControllerSystem.h"
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
		InitializeResources();
	}

	World::World(
		SystemManager & systemManager,
		Filesystem::path parentDir,
		string name,
		int seed
	) : World::World(systemManager, parentDir, name)
	{
		
		m_systemManager.AddSystem(std::shared_ptr<System>(new PlayerSystem(&m_systemManager, m_entityManager.get(), 1)));
		m_systemManager.AddSystem(std::shared_ptr<System>(new TerrainSystem(&m_systemManager, m_entityManager.get(), 1, m_width, m_regionWidth, m_directory / "System")));
		m_systemManager.AddSystem(std::shared_ptr<System>(new PlantSystem(&m_systemManager, m_entityManager.get(), 0)));
		m_systemManager.AddSystem(std::shared_ptr<System>(new BuildingSystem(m_entityManager.get(), 0)));
		Generate(seed);
	}

	World::~World()
	{
		auto gui = m_systemManager.GetSystem<GuiSystem>("Gui");
		gui->OpenMenu("loading");
		//----------------------------------------------------------------
		// Halt world systems
		gui->SetTextByID("Message", "Stopping systems ...");
		m_systemManager.Halt<WorldSystem>();
		//----------------------------------------------------------------
		// Wait for all tasks to complete
		gui->SetTextByID("Message", "Waiting for tasks ...");
		TaskManager::Get().WaitForAll();
		
		//----------------------------------------------------------------
		// Update render targets
		m_systemManager.GetSystem<RenderSystem>("Render")->InitializeWorldRendering(nullptr);

		
		
		//----------------------------------------------------------------
		// Save all systems
		gui->SetTextByID("Message", "Saving systems ...");
		m_systemManager.Save();
		//----------------------------------------------------------------
		// Deconstruct the entity manager
		gui->SetTextByID("Message", "Saving entities ...");
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
		gui->SetTextByID("Message", "World Closed");
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
		m_systemManager.GetSystem<GuiSystem>("Gui")->OpenMenu("HUD");
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
		auto gui = m_systemManager.GetSystem<GuiSystem>("Gui");
		gui->OpenMenu("loading");
		gui->SetTextByID("Message", "Generating Player ...");
		m_systemManager.GetSystem<PlayerSystem>("Player")->CreatePlayer();
		gui->SetTextByID("Message", "Generating Terrain ...");
		m_systemManager.GetSystem<TerrainSystem>("Terrain")->Generate([=](float percent,std::string message) {
			gui->SetTextByID("Message", "Generating Terrain : " + message);
			gui->GetStyle("Bar")->Width = to_string(percent * 100) + '%';
			gui->UpdateUI();
		});
		
		
		gui->SetTextByID("Message", "Generating Plants ...");
		m_systemManager.GetSystem<PlantSystem>("Plant")->Generate();
		gui->SetTextByID("Message", "Generation Done");
	}

	bool World::Load()
	{
		auto gui = m_systemManager.GetSystem<GuiSystem>("Gui");
		gui->OpenMenu("loading");
		gui->SetTextByID("Message", "Loading " + m_name + " ...");
		
		gui->BindHandlers();
		
		//----------------------------------------------------------------
		// Add world systems
		m_systemManager.AddSystem(std::shared_ptr<System>(new PlayerSystem(&m_systemManager, m_entityManager.get(), 1)));
		m_systemManager.AddSystem(std::shared_ptr<System>(new TerrainSystem(&m_systemManager, m_entityManager.get(), 1, m_width, m_regionWidth, m_directory / "System")));
		m_systemManager.AddSystem(std::shared_ptr<System>(new BuildingSystem(m_entityManager.get(), 0)));
		m_systemManager.AddSystem(std::shared_ptr<System>(new MovementControllerSystem(m_entityManager.get())));
		m_systemManager.AddSystem(std::shared_ptr<System>(new CollisionSystem(&m_systemManager, m_entityManager.get(), 1)));
		
		m_systemManager.AddSystem(std::shared_ptr<System>(new MovementSystem(m_entityManager.get(), 1)));
		

		//m_systemManager.AddSystem(std::shared_ptr<System>(new ActionSystem(&m_systemManager, m_entityManager.get(), 10)));
		m_systemManager.AddSystem(std::shared_ptr<System>(new PlantSystem(&m_systemManager, m_entityManager.get(), 2)));
		//m_systemManager.AddSystem(std::shared_ptr<System>(new ItemSystem(m_entityManager.get(), 0)));
		//m_systemManager.AddSystem(std::shared_ptr<System>(new BuildingSystem(m_entityManager.get(), 0)));
		//m_systemManager.AddSystem(std::shared_ptr<System>(new ResourceSystem(m_entityManager.get(), 0)));

		IEventManager::RegisterHandler(EventTypes::Render_Synced, std::function<void(void)>([=] {
			ResumeGame();
		}));
		IEventManager::RegisterHandler(EventTypes::Game_Pause, std::function<void(void)>([=] {
			PauseGame();
		}));
		IEventManager::RegisterHandler(EventTypes::Game_Resume, std::function<void(void)>([=] {
			ResumeGame();
		}));
		auto position = m_entityManager->PlayerPos();
		Render();
		gui->SetTextByID("Message", "Loading Entities ...");
		IEventManager::Invoke(EventTypes::Movement_PlayerMoved, position.x, position.z);

		return true;
	}

	void World::Render()
	{
		auto renderSystem = m_systemManager.GetSystem<RenderSystem>("Render");
		renderSystem->InitializeWorldRendering(m_entityManager.get());
	}

	/*void World::SetMousePos(Vector2 pos)
	{
		auto playerSystem = m_systemManager.GetSystem<PlayerSystem>("Player");
		if (playerSystem && !m_paused) {
			playerSystem->SetMousePos(pos);
		}
	}*/
	void World::InitializeResources()
	{
		//----------------------------------------------------------------
		// Reset event handlers
		IEventManager::NewVersion();
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
		m_entityManager.reset(new WEM(componentDir, m_width, m_regionWidth, worldConfig["loadRange"].To<float>()));
	}
}