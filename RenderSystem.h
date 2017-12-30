#pragma once
#include "System.h"
#include "VBO.h"
#include "Model.h"
#include "AssetManager.h"
class RenderSystem :
	public System
{
public:
	RenderSystem(
		shared_ptr<EntityManager> & entityManager, 
		vector<string> & components, 
		unsigned short updatePeriod,
		HWND window, int width, int height,
		Filesystem::path worldAssets
	);
	// Inherited via System
	virtual void Update(double & elapsed) override;
	virtual void SyncEntities() override;
	void SetViewport(int width, int height);
	~RenderSystem();
private:
	
	shared_ptr<Entity> m_player;
	
	// DirectX
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_waterLayout;
	vector<string>								m_effectOrder;
	
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

	std::shared_ptr<DirectX::CommonStates>			m_states;

	
	// Matricies
	SimpleMath::Matrix	m_worldMatrix;
	SimpleMath::Matrix 	m_viewMatrix;
	SimpleMath::Matrix  m_projMatrix;
	void UpdateEffectMatricies(std::shared_ptr<DGSLEffect> effect, int backBufferWidth, int backBufferHeight);
	// Initializes window-dependent resources
	void CreateResources();
	// Initialzies window-independent resources
	void CreateDevice();
	// Reset resources
	void OnDeviceLost();

	DirectX::XMMATRIX GetViewMatrix();
	//----------------------------------------------------------------
	// Components::VBO
	const unsigned long m_VBOmask;
	std::map<string, vector<shared_ptr<Components::VBO>>> m_VBOs;
	
	void RenderVBO(shared_ptr<Components::VBO> vbo);
	//----------------------------------------------------------------
	// Components::Model using DirectX::Model
	const unsigned long m_ModelMask;
	std::map<string, vector<shared_ptr<Components::Model>>> m_Models;

	//----------------------------------------------------------------
	// Rendering
	void Clear();
	void SetStates();
	void Render();
	void Present();

	// Inherited via System
	virtual string Name() override;

};

