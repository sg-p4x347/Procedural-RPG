#pragma once
#include "WorldDomain.h"
#include "SystemManager.h"
namespace world {
	class World {
	public:
		World(
			SystemManager & systemManager,
			Filesystem::path parentDir,
			string name
		);
		World(
			SystemManager & systemManager,
			Filesystem::path parentDir,
			string name,
			int seed
		);
		~World();
		// Properties
		int GetWidth();
		// Simulation state
		void PauseGame();
		void ResumeGame();
		void TogglePause();
		// Generates a world
		void Generate(int seed);
		// Loads this world from file
		bool Load();
		// Allows the RenderSystem to render the world
		void Render();
		// Input
		void SetMousePos(Vector2 pos);

	private:
		void InitializeResources();
		//----------------------------------------------------------------
		// Systems
		SystemManager & m_systemManager;
		//----------------------------------------------------------------
		// Entities
		unique_ptr<WEM> m_entityManager;
		//----------------------------------------------------------------
		// Metainformation
		Filesystem::path m_directory;
		string m_name;
		int m_width;
		int m_regionWidth;
		//----------------------------------------------------------------
		// World control
		bool m_inWorld;

		void HaltWorldSystems();
		void RunWorldSystems();
		bool m_paused;



	};
}

