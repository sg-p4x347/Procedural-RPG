#pragma once
#include "WorldSystem.h"
#include "GuiSystem.h"
#include "PositionNormalTextureVBO.h"
#include "PositionNormalTextureTangentColorVBO.h"
#include "Model.h"
#include "CompositeModel.h";
#include "AssetManager.h"
class SystemManager;
struct RenderEntityJob {
	world::EntityID entity;
	std::shared_ptr<Model> model;
	Vector3 position;
	XMMATRIX worldMatrix;
};
class RenderSystem :
	public System
{
public:
	RenderSystem(
		SystemManager * systemManager,
		unsigned short updatePeriod,
		HWND window, int width, int height,
		shared_ptr<GuiSystem> guiSystem
	);
	// Inherited via System
	virtual void Update(double & elapsed) override;
	virtual void SyncEntities() override;
	void SetViewport(int width, int height);
	Rectangle GetViewport();
	void InitializeWorldRendering(world::WEM * entityManager);
	void RegisterHandlers();
	~RenderSystem();
private:
	world::WEM * EM;
	shared_ptr<GuiSystem> m_guiSystem;
	SystemManager * SM;
	EntityPtr m_player;
	std::shared_ptr<Model> m_oceanModel;
	std::unique_ptr<world::WorldEntityCache<world::WEM::RegionType, world::Terrain, world::Model, world::Position>> m_terrainEntities;
	const float m_fov;
	const float m_clipNear;
	const float m_clipFar;
	float m_aspectRatio;
	bool m_ready;
	std::mutex m_mutex;
	std::mutex m_syncMutex;
	std::deque<int> m_frameDeltas;
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
	BoundingFrustum m_frustum;
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
	std::map<string, vector<shared_ptr<Components::PositionNormalTextureTangentColorVBO>>> m_VBOs;
	
	void RenderVBO(shared_ptr<Components::PositionNormalTextureTangentColorVBO> vbo);
	//----------------------------------------------------------------
	// Components::Model using DirectX::Model
	typedef std::map<shared_ptr<world::WEM::RegionType>, std::map<world::MaskType,vector<RenderEntityJob>>> ModelInstanceCache;
	ModelInstanceCache m_modelInstances;
	std::set<shared_ptr<world::WEM::RegionType>> m_visibleRegions;
	std::set<world::EntityID> m_tracked;
	//std::map<shared_ptr<Model>, vector<RenderEntityJob>> m_modelInstancesTemp;
	//std::set<world::EntityID> m_trackedTemp;
	void TrackEntity(ModelInstanceCache & modelInstances, shared_ptr<world::WEM::RegionType> region, std::set<world::EntityID> & tracked,world::MaskType signature, world::WorldEntityProxy<world::Model,world::Position> & entity,Vector3 camera, bool ignoreVerticalDistance = false);
	void UpdateVisibleRegions(Vector3 & cameraPosition, Vector3 & cameraRotation);
	bool IsRectVisible(Rectangle & area,Vector2 & observerPos,Vector2 & fovNorm1, Vector2 & fovNorm2);
	bool IsRegionVisible(shared_ptr<world::WEM::RegionType> region, Vector3 & position, Vector3 & rotation, BoundingFrustum & frustum);
	void CreateFromMatrixRH(BoundingFrustum& Out, CXMMATRIX Projection);
	//----------------------------------------------------------------
	// DX::Model

	void RenderModels(Vector3 & cameraPos,world::MaskType signatureMask, bool opaque);
	// Render all opaque or alpha meshes within the model 
	void RenderModel(shared_ptr<DirectX::Model> model, XMMATRIX world,bool opaque);
	void RenderCompositeModel(shared_ptr<CompositeModel> model, Vector3 & position, Vector3 & rotation, bool backfaceCulling);
	void RenderModelMesh(DirectX::ModelMesh * mesh, XMMATRIX world, bool backfaceCulling);
	//----------------------------------------------------------------
	// Entity Rendering
	void Clear();
	void SetStates();
	void Render();
	void Present();
	//----------------------------------------------------------------
	// 2D Rendering
	void RenderGUI();
	void RenderGuiEntityRecursive(EntityPtr entity, vector<EntityPtr> & spriteBatches);
	void SpriteBatchBegin(vector<Rectangle> clippingRects = vector<Rectangle>());
	void SpriteBatchDraw(shared_ptr<Sprite> sprite);
	map<string,shared_ptr<SpriteFont>> m_font;
	shared_ptr<SpriteFont> GetFont(string path,int size);
	void DrawText(string text,string font, Vector2 position,int size,SimpleMath::Color color=SimpleMath::Color(Colors::Black),float layer = 0.f);
	void SpriteBatchEnd();
	// clipping rasterizer state
	ID3D11RasterizerState * m_scissorState;
	// Inherited via System
	virtual string Name() override;
	//----------------------------------------------------------------
	// Logging
	void UpdateFramerate();
};

