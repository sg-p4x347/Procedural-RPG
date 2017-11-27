#pragma once
#include "System.h"
#include "VBO.h"
class RenderSystem :
	public System
{
public:
	RenderSystem(
		shared_ptr<EntityManager> & entityManager, 
		vector<string> & components, 
		unsigned short updatePeriod,
		HWND window, int width, int height
	);
	// Inherited via System
	virtual void Update(double & elapsed) override;

	void SetViewport(int width, int height);
	~RenderSystem();
private:
	
	shared_ptr<Entity> m_player;
	// DirectX
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_waterLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_waterShader;
	std::unique_ptr<DirectX::DGSLEffect>		m_effect;
	vector<string> m_effectNames;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture3;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture4;
	std::shared_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>	m_batch;

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

	std::shared_ptr<DirectX::CommonStates>		m_states;

	std::unique_ptr<DGSLEffectFactory>				m_fxFactory;
	map<string,shared_ptr<DGSLEffect>> m_effects;
	// Matricies
	SimpleMath::Matrix	m_worldMatrix;
	SimpleMath::Matrix 	m_viewMatrix;
	SimpleMath::Matrix  m_projMatrix;
	void UpdateEffectMatricies(int backBufferWidth, int backBufferHeight);
	// Initializes window-dependent resources
	void CreateResources();
	// Initialzies window-independent resources
	void CreateDevice();
	// Reset resources
	void OnDeviceLost();

	DirectX::XMMATRIX GetViewMatrix();
	void RenderVBO(shared_ptr<Components::VBO> vbo, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, bool first);
	void Clear();
	void Present();
	// Component
	const unsigned long m_VBOmask;
	// Inherited via System
	virtual string Name() override;

};

