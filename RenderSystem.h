#pragma once
#include "WorldSystem.h"
#include "GuiSystem.h"
#include "PositionNormalTextureVBO.h"
#include "Model.h"
#include "AssetManager.h"
class RenderSystem :
	public System
{
public:
	RenderSystem(
		unsigned short updatePeriod,
		HWND window, int width, int height,
		shared_ptr<GuiSystem> guiSystem
	);
	// Inherited via System
	virtual void Update(double & elapsed) override;
	virtual void SyncEntities() override;
	void SetViewport(int width, int height);
	Rectangle GetViewport();
	void InitializeWorldRendering(WorldEntityManager * entityManager);
	~RenderSystem();
private:
	WorldEntityManager * EM;
	shared_ptr<GuiSystem> m_guiSystem;
	EntityPtr m_player;
	std::mutex m_mutex;
	// DirectX
	
	vector<string>								m_effectOrder;
	
	std::shared_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>	m_batch;
	std::unique_ptr<SpriteBatch> m_spriteBatch;
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
	void UpdateEffectMatricies(std::shared_ptr<IEffectMatrices> effect, int backBufferWidth, int backBufferHeight);
	// Initializes window-dependent resources
	void CreateResources();
	// Initialzies window-independent resources
	void CreateDevice();
	// Reset resources
	void OnDeviceLost();

	DirectX::XMMATRIX GetViewMatrix();
	//----------------------------------------------------------------
	// Components::VBO
	unsigned long m_VBOmask;
	std::map<string, vector<shared_ptr<Components::PositionNormalTextureVBO>>> m_VBOs;
	
	void RenderVBO(shared_ptr<Components::PositionNormalTextureVBO> vbo);
	//----------------------------------------------------------------
	// Components::Model using DirectX::Model
	unsigned long m_ModelMask;
	std::map<string, vector<shared_ptr<Components::Model>>> m_Models;

	//----------------------------------------------------------------
	// Entity Rendering
	void Clear();
	void SetStates();
	void Render();
	void Present();
	//----------------------------------------------------------------
	// 2D Rendering
	void SpriteBatchBegin();
	void SpriteBatchDraw(shared_ptr<Sprite> sprite);
	shared_ptr<SpriteFont> m_font;
	void SetFont(string path);
	void DrawText(string text, Vector2 position,float size,SimpleMath::Color color=SimpleMath::Color(Colors::Black));
	void SpriteBatchEnd();
	// Inherited via System
	virtual string Name() override;

};

