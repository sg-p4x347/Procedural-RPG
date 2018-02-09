#pragma once
#include "System.h"
#include "WorldEntityManager.h"
#include "EventManager.h"
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
	inline void Remove() {
		for (auto & pair : map<string,shared_ptr<System>>(m_systems)) {
			auto system = dynamic_pointer_cast<SystemType>(pair.second);
			if (system) m_systems.erase(pair.first);
		}
	}
	template <typename SystemType>
	inline void Halt() {
		for (auto & pair : m_systems) {
			auto system = dynamic_pointer_cast<SystemType>(pair.second);
			if (system) system->Halt();
		}
	}
	template <typename SystemType>
	inline void Run() {
		for (auto & pair : m_systems) {
			auto system = dynamic_pointer_cast<SystemType>(pair.second);
			if (system) system->Run();
		}
	}
	EventManager<> & GetEventManager();
private:
	//----------------------------------------------------------------
	// Systems
	shared_ptr<SystemManager> m_this;
	void AddSystem(shared_ptr<System> system);
	map<string, shared_ptr<System>> m_systems;
	//----------------------------------------------------------------
	// Entities
	unique_ptr<WorldEntityManager> m_entityManager;
	//----------------------------------------------------------------
	// Events
	EventManager<> m_events;

};

