#include "pch.h"
#include "RenderSystem.h"
#include "Position.h"

using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;

RenderSystem::RenderSystem(
	shared_ptr<EntityManager>& entityManager, 
	vector<string>& components, 
	unsigned short updatePeriod,
	HWND window, int width, int height
) : System(entityManager,components, updatePeriod), 
m_VBOmask(entityManager->ComponentMask("VBO"))
{
	m_window = window;
	

	CreateDevice();
	SetViewport(width, height);
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::Update(double & elapsed)
{
	//https://stackoverflow.com/questions/17655442/how-can-i-repeat-my-texture-in-dx

	//TODO: Add your rendering code here.
	m_effect->Apply(m_d3dContext.Get());

	// camera
	m_effect->SetView(GetViewMatrix());

	auto sampler = m_states->LinearWrap();

	m_d3dContext->PSSetSamplers(0, 1, &sampler);
	m_d3dContext->RSSetState(m_states->CullCounterClockwise());
	m_d3dContext->IASetInputLayout(m_inputLayout.Get());

	// Do the rendering
	Clear();
	for (shared_ptr<Entity> entity : m_entities) {
		if (entity->HasComponents(m_VBOmask)) {
			RenderVBO(EM->GetComponent<Components::VBO>(entity,"VBO"),m_d3dContext,m_batch);
		}
	}
	Present();
}
void RenderSystem::SetViewport(int width, int height)
{
	m_outputWidth = std::max(width, 1);
	m_outputHeight = std::max(height, 1);
	CreateResources();
}
void RenderSystem::Clear()
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
void RenderSystem::Present()
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

void RenderSystem::CreateResources()
{
	// Clear the previous window size specific context.
	ID3D11RenderTargetView* nullViews[] = { nullptr };
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

	m_effect->SetViewport(float(backBufferWidth), float(backBufferHeight));
	m_projMatrix = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(70.f),
		float(backBufferWidth) / float(backBufferHeight), 0.1f, 8000.f);
	m_effect->SetProjection(m_projMatrix);
	
}

void RenderSystem::CreateDevice()
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
			D3D11_MESSAGE_ID hide[] =
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
	auto blob = DX::ReadData(L"Terrain.cso"); // .cso is the compiled version of the hlsl shader (compiled shader object)
	DX::ThrowIfFailed(m_d3dDevice->CreatePixelShader(&blob.front(), blob.size(),
		nullptr, m_pixelShader.ReleaseAndGetAddressOf()));

	m_effect = std::make_unique<DGSLEffect>(m_d3dDevice.Get(), m_pixelShader.Get());
	m_effect->SetTextureEnabled(true);
	m_effect->SetVertexColorEnabled(true);
	//---Textures---
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(m_d3dDevice.Get(), L"dirt.dds", nullptr,
			m_texture.ReleaseAndGetAddressOf()));

	m_effect->SetTexture(m_texture.Get());

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(m_d3dDevice.Get(), L"grass.dds", nullptr,
			m_texture2.ReleaseAndGetAddressOf()));

	m_effect->SetTexture(1, m_texture2.Get());

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(m_d3dDevice.Get(), L"stone.dds", nullptr,
			m_texture3.ReleaseAndGetAddressOf()));

	m_effect->SetTexture(2, m_texture3.Get());

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(m_d3dDevice.Get(), L"snow.dds", nullptr,
			m_texture4.ReleaseAndGetAddressOf()));

	m_effect->SetTexture(3, m_texture4.Get());
	//---Textures---

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
	m_effect->SetWorld(m_worldMatrix);
}

void RenderSystem::OnDeviceLost()
{
	m_states.reset();
	m_depthStencilView.Reset();
	m_renderTargetView.Reset();
	m_swapChain1.Reset();
	m_swapChain.Reset();
	m_d3dContext1.Reset();
	m_d3dContext.Reset();
	m_d3dDevice1.Reset();
	m_d3dDevice.Reset();


	m_effect.reset();
	m_inputLayout.Reset();
	m_texture.Reset();
	m_texture2.Reset();
	m_texture3.Reset();
	m_pixelShader.Reset();
}

DirectX::XMMATRIX RenderSystem::GetViewMatrix()
{
	shared_ptr<Components::Position> position = EM->GetComponent<Components::Position>(EM->Player(),"Position");
	float y = sinf(position->Rot.y);
	float r = cosf(position->Rot.y);
	float z = r*cosf(position->Rot.x);
	float x = r*sinf(position->Rot.x);

	XMVECTOR lookAt = position->Pos + SimpleMath::Vector3(x, y, z);

	XMMATRIX view = XMMatrixLookAtRH(position->Pos, lookAt, SimpleMath::Vector3(0.f, 1.f, 0.f));
	return view;
}

void RenderSystem::RenderVBO(shared_ptr<Components::VBO> vbo, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, shared_ptr<PrimitiveBatch<DirectX::VertexPositionColor>> batch)
{

	// make sure the buffers have been updated
	if (vbo->LODchanged && vbo->Vertices.size() != 0) {
		vbo->CreateBuffers(m_d3dDevice);
		vbo->LODchanged = false;
	}
	// Set vertex buffer stride and offset.
	UINT stride = sizeof(VertexPositionNormalTangentColorTexture);
	UINT offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, vbo->VB.GetAddressOf(), &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(vbo->IB.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw
	deviceContext->DrawIndexed(vbo->Indices.size(), 0, 0);
}

string RenderSystem::Name()
{
	return "Render";
}