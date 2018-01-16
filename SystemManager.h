#pragma once
#include "System.h"
#include "EntityManager.h"
//#include "RenderSystem.h"

class SystemManager
{
public:
	SystemManager(
		HWND window, int width, int height
	);
	~SystemManager();
	// Updates systems according to their update period
	void Tick(double & elapsed);
	void Initialize();
	void LoadWorld(Filesystem::path worldDir);
	void CloseWorld();
	void Save();
	template <typename SystemType>
	inline shared_ptr<SystemType> GetSystem(string name) {
		return dynamic_pointer_cast<SystemType>(m_systems[name]);
	}
	template <typename SystemType>
	inline void HaltAll() {
		for (auto & pair : m_systems) {
			auto worldSystem = dynamic_pointer_cast<WorldSystem>(pair.second);
			if (worldSystem) worldSystem->Halt();
		}
	}
	void RunAll();
private:
	//----------------------------------------------------------------
	// Systems
	void AddSystem(shared_ptr<System> system);
	map<string, shared_ptr<System>> m_systems;
	//----------------------------------------------------------------
	// Entities
	shared_ptr<EntityManager> m_entityManager;
	

};

