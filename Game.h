#pragma once

#include "World.h"
#include "StepTimer.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
__declspec(align(16)) class Game
{
public:

    Game();
	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}
	//--------------------------------
    // Initialization and management
    void Initialize(HWND window, int width, int height);
	//--------------------------------
    // Basic game loop
    void Tick();
	

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

private:
	//--------------------------------
	// The currently loaded world
	unique_ptr<World> m_world;
    void Update(DX::StepTimer const& timer);
	void Render();
	// Generates a world
	void GenerateWorld(int seed,string name, HWND window, int width, int height);
	// Loads a world from file
	void LoadWorld(string name, HWND window, int width, int height);
	//--------------------------------
	// DirectX
    void CreateDevice();
    void CreateResources();
    void OnDeviceLost();

	//--------------------------------
	// Rendering helpers
	void Clear();
	void Present();
	//--------------------------------
    // Device resources.
    HWND                                            m_window;
    int                                             m_outputWidth;
    int                                             m_outputHeight;

    D3D_FEATURE_LEVEL                               m_featureLevel;
    Microsoft::WRL::ComPtr<ID3D11Device>            m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice1;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>     m_d3dContext;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext1;

    Microsoft::WRL::ComPtr<IDXGISwapChain>          m_swapChain;
    Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain1;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

	std::shared_ptr<DirectX::CommonStates>			m_states;
	
	std::unique_ptr<DGSLEffectFactory>				m_fxFactory;

	

	

	
	
	// Input
	std::shared_ptr<DirectX::Keyboard>				m_keyboard;
	std::shared_ptr<DirectX::Mouse>					m_mouse;
    // Rendering loop timer.
    DX::StepTimer                                   m_timer;
};