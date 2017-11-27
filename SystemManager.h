#pragma once
#include "System.h"
#include "EntityManager.h"
//#include "RenderSystem.h"

class SystemManager
{
public:
	SystemManager(
		Filesystem::path directory,
		std::shared_ptr<EntityManager> entityManager,
		HWND window, int width, int height,
		std::shared_ptr<DirectX::Mouse> mouse,
		std::shared_ptr<DirectX::Keyboard> keyboard
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
private:
	//----------------------------------------------------------------
	// Systems
	void AddSystem(shared_ptr<System> system);
	map<string, shared_ptr<System>> m_systems;
	//----------------------------------------------------------------
	// Entities
	shared_ptr<EntityManager> m_entityManager;
	

};

