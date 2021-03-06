#pragma once
#include "SystemManager.h"
#include "StepTimer.h"
#include "World.h"
#include "JsonParser.h"
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
	bool TryGetWorld(world::World *& world);
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
	//void SetMousePos(Vector2 pos);
	static Vector2 MousePos;
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
	unique_ptr<world::World> m_world;
    void Update(double elapsed);

	//----------------------------------------------------------------
	// Input
	std::shared_ptr<DirectX::Keyboard>				m_keyboard;
	Keyboard::KeyboardStateTracker					m_keyboardTracker;
	std::shared_ptr<DirectX::Mouse>					m_mouse;
    //----------------------------------------------------------------
	// Game loop
    DX::StepTimer                                   m_timer;
	
};