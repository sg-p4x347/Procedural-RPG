#pragma once
#include "SystemManager.h"
#include "StepTimer.h"
#include "World.h"
class Game
{
public:
	static Game& Get();
	void Tick();
	//--------------------------------
    // Initialization and management
    void Initialize(HWND window, int width, int height);
	// gets the directory that contains all saved worlds
	Filesystem::path GetSavesDirectory();
	//----------------------------------------------------------------
	// World
	bool TryGetWorld(World *& world);
	void GenerateWorld(string name, int seed);
	bool LoadWorld(string name = "");
	void CloseWorld();
    //----------------------------------------------------------------
	// Window messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int width, int height);
	void OnQuit();
    // Properties
    void GetDefaultSize( int& width, int& height ) const;
	//----------------------------------------------------------------
	// Input
	void CharTyped(char ch);
	void Backspace();
	static DirectX::Mouse::State MouseState;
	static DirectX::Mouse::ButtonStateTracker MouseTracker;
	static DirectX::Keyboard::State KeyboardState;
	static DirectX::Keyboard::KeyboardStateTracker KeyboardTracker;
private:
	Game();
	//----------------------------------------------------------------
	// Config
	JsonParser m_config;
	
	//----------------------------------------------------------------
	// Systems
	unique_ptr<SystemManager> m_systemManager;

	//----------------------------------------------------------------
	// World
	unique_ptr<World> m_world;
    void Update(DX::StepTimer const& timer);

	//----------------------------------------------------------------
	// Input
	std::shared_ptr<DirectX::Keyboard>				m_keyboard;
	Keyboard::KeyboardStateTracker					m_keyboardTracker;
	std::shared_ptr<DirectX::Mouse>					m_mouse;
    //----------------------------------------------------------------
	// Game loop
    DX::StepTimer                                   m_timer;
	
};