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

	
	AddSystem(std::shared_ptr<System>(new GuiSystem(this, 1)));
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

void SystemManager::Save()
{
	for (auto & system : m_systems) system.second->Save();
}

void SystemManager::AddSystem(shared_ptr<System> system)
{
	m_systems.insert(std::pair<string, std::shared_ptr<System>>(system->Name(),system));
}
