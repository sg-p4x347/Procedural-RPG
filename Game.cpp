//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
//namespace
//{
//#include "dgslsphere.inc"
//}
const VertexPositionNormalTangentColorTexture g_sphereVB[] = {
	{ XMFLOAT3(0,-16,0), XMFLOAT3(0,1,0), XMFLOAT4(1,0,0,1),
	0xFFFFFFFF, XMFLOAT2(0,0) },
	{ XMFLOAT3(4,-16,0), XMFLOAT3(0,1,0), XMFLOAT4(1,0,0,1),
	0xFFFFFFFF, XMFLOAT2(1,0) },
	{ XMFLOAT3(0,-16,4), XMFLOAT3(0,1,0), XMFLOAT4(1,0,0,1),
	0xFFFFFFFF, XMFLOAT2(0,1) }
};
const uint16_t g_sphereIB[] = {
	0,1,2
};
using Microsoft::WRL::ComPtr;

Game::Game() :
    m_window(0),
    m_outputWidth(800),
    m_outputHeight(600),
    m_featureLevel(D3D_FEATURE_LEVEL_9_1)
{
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_window = window;
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    CreateDevice();

    CreateResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);
	m_mouse->SetMode(DirectX::Mouse::Mode::MODE_RELATIVE);
	createWorld(347, "testWorld");
	loadWorld();
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    
}

// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsed = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
	float time = float(timer.GetTotalSeconds());
	// DX Input
	auto keyboard = m_keyboard->GetState();
	if (keyboard.Escape)
		PostQuitMessage(0);

	auto mouse = m_mouse->GetState();
	// Update the world
	m_world->update(elapsed,mouse,keyboard);
}

// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    //TODO: Add your rendering code here.
	
	m_effect->Apply(m_d3dContext.Get());

	
	// camera
	m_viewMatrix = m_world->getPlayer()->getViewMatrix();
	m_effect->SetView(m_viewMatrix);
	
	// Render Origin
	//m_origin->Draw(m_worldMatrix, m_viewMatrix, m_projMatrix);
	
	// Render regions
	auto sampler = m_states->LinearWrap();

	m_d3dContext->PSSetSamplers(0, 1, &sampler);
	m_d3dContext->RSSetState(m_states->CullCounterClockwise());
	m_d3dContext->IASetInputLayout(m_inputLayout.Get());

	shared_ptr<CircularArray> regions = m_world->getRegions();
	for (int i = regions->size() - 1; i >= 0; i--) {
		if (!regions->data[i].isNull()) {
			regions->data[i].render(m_d3dContext,m_batch.get());
		}
	}
	// DO NOT DELETE
	Present();
}
// creates a new world file
void Game::createWorld(int seed, string name) {
	int regionWidth = 32;
	// create the relative path
	m_workingPath = "saves/" + name + "/";
	// create the save directory
	CreateDirectory(wstring(m_workingPath.begin(), m_workingPath.end()).c_str(), NULL);
	
	// seed the RNG
	srand(seed);
	// generate the terrain distribution maps
	Distribution *continentMap = new Distribution(32, 0.3, 2, true);
	Distribution *biomeMap = new Distribution(32, 1.0, 3, false);
	Distribution *terrain = new Distribution(8, 0.45, 5, continentMap, biomeMap);

	unsigned int vertexCount = (regionWidth + 1)*(regionWidth + 1);
	unsigned int regionCount = (terrain->getWidth() / regionWidth)*(terrain->getWidth() / regionWidth);
	//-------------------------------
	// save terrain to binary file
	//-------------------------------
	unsigned char *terrainBuffer = new unsigned char[vertexCount*regionCount * sizeof(short)];
	char *normalBuffer = new char[vertexCount*regionCount * 3]; // X (8 bits) Y (8 bits) Z (8 bits)
	// iterate through each region
	unsigned int index = 0;
	for (unsigned short regionY = 0; regionY < (terrain->getWidth() / regionWidth); regionY++) {
		for (unsigned short regionX = 0; regionX < (terrain->getWidth() / regionWidth); regionX++) {
			// push each vertex in the region
			for (unsigned short vertY = 0; vertY <= regionWidth; vertY++) {
				for (unsigned short vertX = 0; vertX <= regionWidth; vertX++) {
					float vertex = terrain->points[regionX*regionWidth + vertX][regionY*regionWidth + vertY];
					terrainBuffer[index * 2] = short(vertex*10) & 0xff;
					terrainBuffer[index * 2 + 1] = (short(vertex*10) >> 8) & 0xff;
					
					
					// normals
					float left = regionX*regionWidth + vertX - 1 >= 0 ? terrain->points[regionX*regionWidth + vertX - 1][regionY*regionWidth + vertY] : vertex;
					float right = regionX*regionWidth + vertX + 1 <= terrain->getWidth() ? terrain->points[regionX*regionWidth + vertX + 1][regionY*regionWidth + vertY] : vertex;
					float up = regionY*regionWidth + vertY + 1 <= terrain->getWidth() ? terrain->points[regionX*regionWidth + vertX][regionY*regionWidth + vertY + 1] : vertex;
					float down = regionY*regionWidth + vertY - 1 >= 0 ? terrain->points[regionX*regionWidth + vertX][regionY*regionWidth + vertY - 1] : vertex;

					Vector3 normal = Vector3(left - right, 2, down - up);
					normal.Normalize();
					// scale the vector up into the characters range
					normal *= 128;
					normalBuffer[index * 3] = char(normal.x);
					normalBuffer[index * 3 + 1] = char(normal.y);
					normalBuffer[index * 3 + 2] = char(normal.z);

					// update vertex index
					index++;
				}
			}
		}
	}
	//-----------------------------------------------
	// output file stream
	//-----------------------------------------------

	// heightMap
	ofstream terrainFile(m_workingPath + "terrain.bin", ios::binary);
	terrainFile.seekp(0);
	if (terrainFile.is_open()) {
		// write the data
		terrainFile.write((const char *)terrainBuffer, vertexCount*regionCount * sizeof(short));
	}
	terrainFile.close();

	// normalMap
	ofstream normalFile(m_workingPath + "normal.bin", ios::binary);
	normalFile.seekp(0);
	if (normalFile.is_open()) {
		// write the data
		normalFile.write((const char *)normalBuffer, vertexCount*regionCount * 3);
	}
	normalFile.close();

	delete[] terrainBuffer;
}
void Game::createPlayer(string name) {

}
void Game::loadWorld() {
	m_world = unique_ptr<World>(new World());
	m_world->init(m_d3dDevice.Get(),512, 32, 16, "saves/testWorld/" );
}
// Helper method to clear the back buffers.
void Game::Clear()
{
    // Clear the views.
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::CornflowerBlue);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    // Set the viewport.
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
    m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void Game::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        OnDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
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
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int width, int height)
{
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    CreateResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
    UINT creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	static const D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

    // Create the DX11 API device object, and get a corresponding context.
    HRESULT hr = D3D11CreateDevice(
        nullptr,                                // specify nullptr to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        m_d3dDevice.ReleaseAndGetAddressOf(),   // returns the Direct3D device created
        &m_featureLevel,                        // returns feature level of device created
        m_d3dContext.ReleaseAndGetAddressOf()   // returns the device immediate context
        );

    if (hr == E_INVALIDARG)
    {
        // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it.
        hr = D3D11CreateDevice(nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creationFlags,
            &featureLevels[1],
            _countof(featureLevels) - 1,
            D3D11_SDK_VERSION,
            m_d3dDevice.ReleaseAndGetAddressOf(),
            &m_featureLevel,
            m_d3dContext.ReleaseAndGetAddressOf()
            );
    }

    DX::ThrowIfFailed(hr);

#ifndef NDEBUG
    ComPtr<ID3D11Debug> d3dDebug;
    if (SUCCEEDED(m_d3dDevice.As(&d3dDebug)))
    {
        ComPtr<ID3D11InfoQueue> d3dInfoQueue;
        if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
        {
#ifdef _DEBUG
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
            D3D11_MESSAGE_ID hide [] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                // TODO: Add more message IDs here as needed.
            };
            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif

    // DirectX 11.1 if present
    if (SUCCEEDED(m_d3dDevice.As(&m_d3dDevice1)))
        (void)m_d3dContext.As(&m_d3dContext1);

    // TODO: Initialize device dependent objects here (independent of window size).
	m_states = std::make_unique<CommonStates>(m_d3dDevice.Get());

	// Create DGSL Effect
	auto blob = DX::ReadData(L"Color.cso");
	DX::ThrowIfFailed(m_d3dDevice->CreatePixelShader(&blob.front(), blob.size(),
		nullptr, m_pixelShader.ReleaseAndGetAddressOf()));

	m_effect = std::make_unique<DGSLEffect>(m_d3dDevice.Get(), m_pixelShader.Get());
	m_effect->SetTextureEnabled(true);
	m_effect->SetVertexColorEnabled(true);

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(m_d3dDevice.Get(), L"Texture.dds", nullptr,
			m_texture.ReleaseAndGetAddressOf()));

	m_effect->SetTexture(m_texture.Get());

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(m_d3dDevice.Get(), L"grass.dds", nullptr,
			m_texture2.ReleaseAndGetAddressOf()));

	m_effect->SetTexture(1, m_texture2.Get());

	m_effect->EnableDefaultLighting();

	void const* shaderByteCode;
	size_t byteCodeLength;

	m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	DX::ThrowIfFailed(m_d3dDevice->CreateInputLayout(
		VertexPositionNormalTangentColorTexture::InputElements,
		VertexPositionNormalTangentColorTexture::InputElementCount,
		shaderByteCode, byteCodeLength,
		m_inputLayout.ReleaseAndGetAddressOf()));

	// Matricies
	m_worldMatrix = Matrix::Identity;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews [] = { nullptr };
    m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_d3dContext->Flush();

    UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
    UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
    DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    UINT backBufferCount = 2;

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_swapChain)
    {
        HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            OnDeviceLost();

            // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
    else
    {
        // First, retrieve the underlying DXGI Device from the D3D Device.
        ComPtr<IDXGIDevice1> dxgiDevice;
        DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

        // Identify the physical adapter (GPU or card) this device is running on.
        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

        // And obtain the factory object that created it.
        ComPtr<IDXGIFactory1> dxgiFactory;
        DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

        ComPtr<IDXGIFactory2> dxgiFactory2;
        if (SUCCEEDED(dxgiFactory.As(&dxgiFactory2)))
        {
            // DirectX 11.1 or later

            // Create a descriptor for the swap chain.
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
            swapChainDesc.Width = backBufferWidth;
            swapChainDesc.Height = backBufferHeight;
            swapChainDesc.Format = backBufferFormat;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = backBufferCount;

            DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = { 0 };
            fsSwapChainDesc.Windowed = TRUE;

            // Create a SwapChain from a Win32 window.
            DX::ThrowIfFailed(dxgiFactory2->CreateSwapChainForHwnd(
                m_d3dDevice.Get(),
                m_window,
                &swapChainDesc,
                &fsSwapChainDesc,
                nullptr,
                m_swapChain1.ReleaseAndGetAddressOf()
                ));

            DX::ThrowIfFailed(m_swapChain1.As(&m_swapChain));
        }
        else
        {
            DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
            swapChainDesc.BufferCount = backBufferCount;
            swapChainDesc.BufferDesc.Width = backBufferWidth;
            swapChainDesc.BufferDesc.Height = backBufferHeight;
            swapChainDesc.BufferDesc.Format = backBufferFormat;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.OutputWindow = m_window;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.Windowed = TRUE;

            DX::ThrowIfFailed(dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &swapChainDesc, m_swapChain.ReleaseAndGetAddressOf()));
        }

        // This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
        DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

    // Create a view interface on the rendertarget to use on bind.
    DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

    // TODO: Initialize windows-size dependent objects here.
	m_origin = GeometricPrimitive::CreateSphere(m_d3dContext.Get());
	m_projMatrix = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(70.f),
		float(backBufferWidth) / float(backBufferHeight), 0.1f, 512.f);

	m_effect->SetViewport(float(backBufferWidth), float(backBufferHeight));

	m_effect->SetView(m_viewMatrix);
	m_effect->SetProjection(m_projMatrix);
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
	m_states.reset();
	m_effect.reset();
	m_inputLayout.Reset();
	m_texture.Reset();
	m_texture2.Reset();
	m_pixelShader.Reset();

    m_depthStencilView.Reset();
    m_renderTargetView.Reset();
    m_swapChain1.Reset();
    m_swapChain.Reset();
    m_d3dContext1.Reset();
    m_d3dContext.Reset();
    m_d3dDevice1.Reset();
    m_d3dDevice.Reset();

    CreateDevice();

    CreateResources();
}