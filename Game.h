#pragma once

#include "pch.h"
#include "World.h"
#include "Distribution.h"
#include "Utility.h"
#include "d3dclass.h"
#include "ShaderManager.h"
#include "ColorShader.h"
#include "inputclass.h"

using namespace Utility;
using namespace DirectX;
using namespace std;

class Game {
	public:
		// constructors
		Game(unsigned int worldWidthIn, unsigned int regionWidthIn, unsigned int loadWidthIn);
		~Game();
		// DirectX
		HRESULT InitDirectX();
		void CleanupDevice();
		// shaders
		HRESULT InitShaders();
		// Windows
		HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
		// Main game loop
		MSG initGameLoop(MSG msg);
		// generator
		void createWorld(int seed, string name);
		void createPlayer(string name);
		// loading from files
		void loadWorld();

	private:
		// game loop
		bool runGame = true;
		float timeStep = 1000 / 60; //  in milliseconds
		// world constants
		unsigned int worldWidth;
		unsigned int regionWidth;
		unsigned int loadWidth;
		string workingPath;
		//---------------------------
		// The currently loaded world
		//---------------------------
		World world;
		//--------------------
		// Direct 3D Interface
		//--------------------
		InputClass * input;
		D3DClass * direct3D;
		ShaderManager * shaderManager;
		// rendering and updating
		HRESULT render();
		void update(double elapsed);
		
		HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

		HINSTANCE               g_hInst = nullptr;
		HWND                    g_hWnd = nullptr;
		D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
		D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
		ID3D11Device*           g_pd3dDevice = nullptr;
		ID3D11Device1*          g_pd3dDevice1 = nullptr;
		ID3D11DeviceContext*    g_pImmediateContext = nullptr;
		ID3D11DeviceContext1*   g_pImmediateContext1 = nullptr;
		IDXGISwapChain*         g_pSwapChain = nullptr;
		IDXGISwapChain1*        g_pSwapChain1 = nullptr;
		ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
		ID3D11Texture2D*        g_pDepthStencil = nullptr;
		ID3D11DepthStencilView* g_pDepthStencilView = nullptr;
		ID3D11VertexShader*     g_pVertexShader = nullptr;
		ID3D11PixelShader*      g_pPixelShader = nullptr;
		ID3D11InputLayout*      g_pVertexLayout = nullptr;
		ID3D11Buffer*           g_pVertexBuffer = nullptr;
		ID3D11Buffer*           g_pIndexBuffer = nullptr;
		ID3D11Buffer*           g_pConstantBuffer = nullptr;
		XMMATRIX                g_World;
		XMMATRIX                g_View;
		XMMATRIX                g_Projection;
		unsigned int			indexSize = 0;
		Float4					g_Eye;
		
};

