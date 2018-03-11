//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "Utility.h"
#include "RenderSystem.h"
#include "TerrainSystem.h"
#include "PlayerSystem.h"
#include "AssetManager.h"
#include "GuiSystem.h"
#include "IEventManager.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

DirectX::Mouse::State Game::MouseState = DirectX::Mouse::State();
DirectX::Keyboard::State Game::KeyboardState = DirectX::Keyboard::State();
DirectX::Mouse::ButtonStateTracker Game::MouseTracker = DirectX::Mouse::ButtonStateTracker();
DirectX::Keyboard::KeyboardStateTracker Game::KeyboardTracker = DirectX::Keyboard::KeyboardStateTracker();


Game::Game()
{
	m_config = JsonParser(ifstream("Config/game.json"));
	AssetManager::Get()->SetAssetDir("Assets");
}

Game & Game::Get()
{
	static Game instance;
	return instance;
}

void Game::Initialize(HWND window, int width, int height)
{
	//----------------------------------------------------------------
	// Initialize the SystemManager
	m_systemManager = std::unique_ptr<SystemManager>(new SystemManager(window, width, height));
	//----------------------------------------------------------------
	// Input
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);
	//----------------------------------------------------------------
	// Game timer
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    //----------------------------------------------------------------
	// Open the main menu
	m_systemManager->GetSystem<GuiSystem>("Gui")->OpenMenu("main");
} 
 
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });
}



// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    double elapsed = timer.GetElapsedSeconds();

    // TODO: Add your game logic here.
	// DX Input
	MouseState = m_mouse->GetState();
	KeyboardState = m_keyboard->GetState();
	MouseTracker.Update(MouseState);
	KeyboardTracker.Update(KeyboardState);

	if (m_world && KeyboardTracker.pressed.Escape) {
		m_world->TogglePause();
	}

	// Update the systems
	m_systemManager->Tick(elapsed);
}
Filesystem::path Game::GetSavesDirectory()
{
	return Filesystem::path("Saves");
}

bool Game::TryGetWorld(World *& world)
{
	if (m_world) {
		world = m_world.get();
		return true;
	}
	return false;
}

void Game::GenerateWorld(string name, int seed)
{
	// Remove existing directory if any
	try {
		Filesystem::remove_all(GetSavesDirectory() / name);
	}
	catch (std::exception ex) {
		Utility::OutputException(ex.what());
	}
	m_world = std::make_unique<World>(*m_systemManager, GetSavesDirectory(),name, seed);
}

bool Game::LoadWorld(string name)
{
	try {
		if (name == "") {
			name = (string)m_config["CurrentWorld"];
			if (name != "") {
				return LoadWorld((string)m_config["CurrentWorld"]);
			}
		}
		else {
			m_config.Set("CurrentWorld", name);
			m_world = std::make_unique<World>(*m_systemManager, GetSavesDirectory(), name);
			//m_world->Load();
			m_world->ResumeGame();
			return true;
		}
	}
	catch (std::exception e) {
		m_systemManager->GetSystem<GuiSystem>("Gui")->DisplayException(e);
	}
	return false;
}

void Game::CloseWorld()
{
	m_world.reset();
}

// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.

}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
	IEventManager::Invoke(EventTypes::Game_Suspend);
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
	IEventManager::Invoke(EventTypes::Game_Resume);
}

void Game::OnWindowSizeChanged(int width, int height)
{
	if (m_systemManager) m_systemManager->GetSystem<RenderSystem>("Render")->SetViewport(width, height);
}

void Game::OnQuit()
{
	if (m_systemManager) m_systemManager->Save();
	m_config.Export(ofstream("Config/game.json"));
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 1920;
    height = 1080;
}

void Game::CharTyped(char ch)
{
	m_systemManager->GetSystem<GuiSystem>("Gui")->CharTyped(ch);
}

void Game::Backspace()
{
	m_systemManager->GetSystem<GuiSystem>("Gui")->Backspace();
}