//
// Game.h
//

#pragma once

#include "World.h"
#include "Distribution.h"
#include "Utility.h"
#include "StepTimer.h"
// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game
{
public:

    Game();

    // Initialization and management
    void Initialize(HWND window, int width, int height);

	// generator
	void createWorld(int seed, string name);
	void createPlayer(string name);

	// loading from files
	void loadWorld();

    // Basic game loop
    void Tick();
    void Render();

    // Rendering helpers
    void Clear();
    void Present();

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

private:
	string m_workingPath;
	//---------------------------
	// The currently loaded world
	unique_ptr<World> m_world;
	//---------------------------
    void Update(DX::StepTimer const& timer);

    void CreateDevice();
    void CreateResources();

    void OnDeviceLost();

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
	// Matricies
	DirectX::SimpleMath::Matrix												m_worldMatrix;
	DirectX::XMMATRIX 														m_viewMatrix;
	DirectX::SimpleMath::Matrix												m_projMatrix;
	// DirectX Pipeline
	std::unique_ptr<DirectX::CommonStates>									m_states;
	std::unique_ptr<DirectX::DGSLEffect>									m_effect;

	std::unique_ptr<DirectX::GeometricPrimitive>							m_origin;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>						m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>						m_texture2;

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>	m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>								m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>								m_pixelShader;
	// Input
	std::unique_ptr<DirectX::Keyboard>				m_keyboard;
	std::unique_ptr<DirectX::Mouse>					m_mouse;
    // Rendering loop timer.
    DX::StepTimer                                   m_timer;
};