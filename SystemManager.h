#pragma once
#include "System.h"
#include "WorldEntityManager.h"
#include "EventManager.h"

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
	void Save();
	template <typename SystemType>
	inline shared_ptr<SystemType> GetSystem(string name) {
		return dynamic_pointer_cast<SystemType>(m_systems[name]);
	}
	void AddSystem(shared_ptr<System> system,bool render=false);
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
private:
	//----------------------------------------------------------------
	// Systems
	
	map<string, shared_ptr<System>> m_systems;
	vector<shared_ptr<System>> m_tickSequence;
	vector<shared_ptr<System>> m_renderSequence;
};

