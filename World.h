#pragma once
#include "WorldEntityManager.h"
class SystemManager;
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

	// Simulation state
	void PauseGame();
	void ResumeGame();
	void TogglePause();
	// Generates a world
	void Generate(int seed);
	// Loads this world from file
	bool Load();
	

private:
	//----------------------------------------------------------------
	// Systems
	SystemManager & m_systemManager;
	//----------------------------------------------------------------
	// Entities
	unique_ptr<WorldEntityManager> m_entityManager;
	//----------------------------------------------------------------
	// Metainformation
	Filesystem::path m_directory;
	string m_name;
	//----------------------------------------------------------------
	// World control
	bool m_inWorld;

	void HaltWorldSystems();
	void RunWorldSystems();
	bool m_paused;

	
	
};

