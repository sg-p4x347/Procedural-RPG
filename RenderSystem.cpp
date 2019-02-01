#include "pch.h"
#include "Game.h"
#include "RenderSystem.h"
#include "Position.h"
#include "Utility.h"
#include "AssetManager.h"
#include "GuiText.h"
#include "CustomEffect.h"
#include "GuiChildren.h"
#include <thread>
#include "SystemManager.h"
#include "BuildingSystem.h"
#include "IEventManager.h"
#include "TaskManager.h"
#include "TerrainSystem.h"

using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;


RenderSystem::RenderSystem(
	SystemManager * systemManager,
	unsigned short updatePeriod,
	HWND window, int width, int height,
	shared_ptr<GuiSystem> guiSystem
) : System(updatePeriod,false),
m_guiSystem(guiSystem),
EM(nullptr),
SM(systemManager),
m_fov(XMConvertToRadians(70.f)),
m_clipNear(0.01f),
m_clipFar(8000.f),
m_wireframe(false),
m_ready(false)

{
	
	m_window = window;
	m_worldMatrix = Matrix::Identity;

	
	CreateDevice();
	
	SetViewport(width, height);

	// clipping state
	CD3D11_RASTERIZER_DESC rsDesc(D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE,
		0, 0.f, 0.f, TRUE, TRUE, TRUE, FALSE);
	m_scissorState = nullptr;
	m_d3dDevice->CreateRasterizerState(&rsDesc, &m_scissorState);
	for (int i = 0; i < 20; i++) {
		m_frameDeltas.push_back(0);
	}
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::Update(double & elapsed)
{
	//----------------------------------------------------------------
	// Update User controlled settings
	if (Game::Get().KeyboardTracker.IsKeyPressed(Keyboard::Keys::F2)) {
		m_wireframe = !m_wireframe;
	}

	// Clear the screen
	Clear();

	SetStates();
	if (EM) Render();
	
	// Update and render GUI
	RenderGUI();

	// Present the backbuffer to the screen
	Present();
	UpdateFramerate();
}
void RenderSystem::Render()
{
	
	Vector3 cameraPos;
	Vector3 cameraRot;
	Vector3 lookAt;
	world::MaskType mask = EM->GetMask<world::Position, world::Player>();
	TaskManager::Get().RunSynchronous(Task([=,&cameraPos,&cameraRot,&lookAt] {
		auto player = EM->GetEntity<world::Position, world::Player>(EM->PlayerID());
		world::Position & position = player->Get<world::Position>();
		world::Player & playerComp = player->Get<world::Player>();
		float y = sinf(playerComp.CameraPitch);
		float r = cosf(playerComp.CameraPitch);
		float z = r * cosf(position.Rot.y);
		float x = r * sinf(position.Rot.y);

		
		cameraRot = Vector3(position.Rot.y, 0.f, playerComp.CameraPitch);
		

		switch (playerComp.InteractionState) {
		case world::Player::InteractionStates::FirstPerson:
			cameraPos = position.Pos + Vector3(0.f, 1.7f, 0.f);
			lookAt = cameraPos + SimpleMath::Vector3(x, y, z);
			break;
		case world::Player::InteractionStates::ThirdPerson:
			lookAt = position.Pos + Vector3(0.f, 1.7f, 0.f);
			cameraPos = lookAt - Vector3(x, y, z) * 3.f;
			
			break;
		}
	}, mask, mask));
		
	XMMATRIX view = XMMatrixLookAtRH(cameraPos, lookAt, SimpleMath::Vector3(0.f, 1.f, 0.f));
	m_viewMatrix = view;
	XMMATRIX ProjectionMatrixLH = XMMatrixPerspectiveFovLH(m_fov, m_aspectRatio, m_clipNear, m_clipFar);
	XMMATRIX ViewMatrixLH = XMMatrixLookAtLH(cameraPos, lookAt, SimpleMath::Vector3(0.f, 1.f, 0.f));
	XMVECTOR Determinant;
	XMMATRIX InvViewMatrixLH = XMMatrixInverse(&Determinant, ViewMatrixLH);
	m_frustum = BoundingFrustum();
	m_frustum.CreateFromMatrix(m_frustum, ProjectionMatrixLH);
	m_frustum.Transform(m_frustum, InvViewMatrixLH);
	// Update the set of visible regions

	// Cull non-visible regions
	UpdateVisibleRegions(cameraPos, cameraRot);
	vector<shared_ptr<world::WEM::RegionType>> regions(m_visibleRegions.begin(),m_visibleRegions.end());
	JobCaches && dynamicJobs = CreateDynamicJobs();
	JobCaches && gridJobs = CreateGridJobs(cameraPos,m_frustum);
	// Sort the regions front to back
	DepthSort(regions, Vector2(cameraPos.x,cameraPos.z));
	// Iterate front to back
	for (auto & region : regions) {
		vector<RenderJob> opaque = m_jobs[region].opaque;
		auto it = dynamicJobs.find(region);
		if (it != dynamicJobs.end()) {
			opaque.insert(opaque.end(), it->second.opaque.begin(), it->second.opaque.end());
		}
		auto gridIt = gridJobs.find(region);
		if (gridIt != gridJobs.end()) {
			opaque.insert(opaque.end(), gridIt->second.opaque.begin(), gridIt->second.opaque.end());
		}
		// Render all visible opaque jobs
		RenderTerrain(region, false);
		RenderJobs(opaque, cameraPos,false);
			
	}
	// Iterate back to front
	for (auto & regionIt = regions.rbegin(); regionIt != regions.rend(); regionIt++) {
		shared_ptr<world::WEM::RegionType> & region = *regionIt;
		vector<RenderJob> alpha = m_jobs[region].alpha;
		auto it = dynamicJobs.find(region);
		if (it != dynamicJobs.end()) {
			alpha.insert(alpha.end(), it->second.alpha.begin(), it->second.alpha.end());
		}
		auto gridIt = gridJobs.find(region);
		if (gridIt != gridJobs.end()) {
			alpha.insert(alpha.end(), gridIt->second.alpha.begin(), gridIt->second.alpha.end());
		}
		// Render all visible alpha jobs
		RenderTerrain(region, true);
		RenderJobs(alpha, cameraPos,true);
			
	}
	//----------------------------------------------------------------
	// Render the ocean
	RenderModel(m_oceanModel, XMMatrixMultiply(XMMatrixTranslationFromVector(Vector3(EM->GetWorldWidth() / 2, 0.05f, EM->GetWorldWidth() / 2)), m_worldMatrix), false);

}
void RenderSystem::SetViewport(int width, int height)
{
	m_aspectRatio = float(width) / float(height);
	m_outputWidth = std::max(width, 1);
	m_outputHeight = std::max(height, 1);
	m_projMatrix = Matrix::CreatePerspectiveFieldOfView(m_fov,m_aspectRatio, m_clipNear, m_clipFar);
	CreateResources();
}

Rectangle RenderSystem::GetViewport()
{
	return Rectangle(0,0, m_outputWidth,m_outputHeight);
}

void RenderSystem::InitializeWorldRendering(world::WEM * entityManager)
{
	EM = entityManager;
	if (entityManager) {
		auto ocean = AssetManager::Get()->GetModel("ocean");
		if (ocean) m_oceanModel = ocean->GetModel(AssetManager::Get()->GetDevice(), AssetManager::Get()->GetFxFactory());
		
		RegisterHandlers();
		InitializeJobCache();
		m_ready = true;
		IEventManager::Invoke(EventTypes::Render_Synced);
	}
	else {
		m_ready = false;
	}
	
}

void RenderSystem::RegisterHandlers()
{
	
	IEventManager::RegisterHandler(EventTypes::Entity_SignatureChanged, std::function<void(shared_ptr<world::WEM::RegionType>, world::EntityID, world::MaskType)>([=](shared_ptr<world::WEM::RegionType> region, world::EntityID id, world::MaskType signature) {
		if (signature) {
			UnloadRegion(region);
			LoadRegion(region);
		}
	}));
	IEventManager::RegisterHandler(EventTypes::WEM_RegionLoaded, std::function<void(shared_ptr<world::WEM::RegionType>)>([=](shared_ptr<world::WEM::RegionType> region) {
		LoadRegion(region);
	}));
	IEventManager::RegisterHandler(EventTypes::WEM_RegionUnloaded, std::function<void(shared_ptr<world::WEM::RegionType>)>([=](shared_ptr<world::WEM::RegionType> region) {
		UnloadRegion(region);
	}));
	
}


void RenderSystem::RenderGUI()
{
	auto currentMenu = m_guiSystem->GetCurrentMenu();
	if (currentMenu) {
		
		vector<EntityPtr> spriteBatches{currentMenu };
		auto handMenu = m_guiSystem->GetHandMenu();
		if (handMenu) {
			spriteBatches.push_back(handMenu);
		}
		while (spriteBatches.size() > 0) {
			vector<EntityPtr> spriteBatchesTemp;
			for (EntityPtr entity : spriteBatches) {
				auto sprite = entity->GetComponent<Sprite>("Sprite");
				
				SpriteBatchBegin(sprite->ClippingRects);
				RenderGuiEntityRecursive(entity, spriteBatchesTemp);
				SpriteBatchEnd();
			}
			spriteBatches = spriteBatchesTemp;
		}
	}
}

void RenderSystem::RenderGuiEntityRecursive(EntityPtr entity, vector<EntityPtr>& spriteBatches)
{
	
	shared_ptr<Sprite> sprite = entity->GetComponent<Sprite>("Sprite");
	if (sprite->Visible) {
		shared_ptr<Text> text = entity->GetComponent<Text>("Text");
		if (sprite) {
			SpriteBatchDraw(sprite);
		}
		if (text) {
			DrawText(text->String, text->Font, text->Position, text->FontSize, text->Color, 1.f / (float)sprite->Zindex);
		}
		shared_ptr<GUI::Children> children = entity->GetComponent<GUI::Children>("Children");
		if (children) {
			for (auto & childID : children->Entities) {
				EntityPtr child;
				if (m_guiSystem->GetEM().Find(childID, child)) {
					auto childSprite = child->GetComponent<Sprite>("Sprite");
					if (childSprite) {
						if (childSprite->ClippingRects.size() > 0) {
							spriteBatches.push_back(child);
						}
						else {
							RenderGuiEntityRecursive(child, spriteBatches);
						}
					}
				}
			}
		}
	}
}

void RenderSystem::SpriteBatchBegin(vector<Rectangle> clippingRects)
{
	m_states = make_shared<CommonStates>(m_d3dDevice.Get());
	if (clippingRects.size() > 0) {
		m_spriteBatch->Begin(SpriteSortMode_BackToFront, m_states->NonPremultiplied(),
			nullptr,
			nullptr,
			m_scissorState,
			[=]() {
			CD3D11_RECT * rects = new CD3D11_RECT[clippingRects.size()];
			for (int i = 0; i < clippingRects.size(); i++) {
				Rectangle rect = clippingRects[i];
				rects[i] = CD3D11_RECT(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
			}
			m_d3dContext->RSSetScissorRects(clippingRects.size(), rects);
			//CD3D11_RECT r(0,0,1920,125);
			//m_d3dContext->RSSetScissorRects(1, &r);
			delete[] rects;
		});
	}
	else {
		m_spriteBatch->Begin(SpriteSortMode_BackToFront, m_states->NonPremultiplied());
	}
}

void RenderSystem::SpriteBatchDraw(shared_ptr<Sprite> sprite)
{
	m_spriteBatch->Draw(
		AssetManager::Get()->GetWicTexture(sprite->BackgroundImage != "" ? sprite->BackgroundImage : "mask.png").Get(),
		sprite->Rect,
		sprite->UseSourceRect ? &(RECT)sprite->SourceRect : nullptr,
		sprite->BackgroundImage != "" ? DirectX::SimpleMath::Color(1.f,1.f,1.f,1.f): sprite->BackgroundColor,
		0.f,
		DirectX::SimpleMath::Vector2::Zero,
		DirectX::SpriteEffects_None,
		1.f/(float)sprite->Zindex
	);
}

shared_ptr<SpriteFont> RenderSystem::GetFont(string path,int size)
{
	return AssetManager::Get()->GetFont(path,size);
}

void RenderSystem::DrawText(string text,string font, Vector2 position,int size, SimpleMath::Color color,float layer)
{
	auto spriteFont = GetFont(font != "" ? font : "Impact", size != 0 ? size : 16);
	if (spriteFont) {
		spriteFont->DrawString(m_spriteBatch.get(), ansi2unicode(text).c_str(), position, color,0.f,Vector2::Zero,Vector2::One,DirectX::SpriteEffects::SpriteEffects_None,layer);
	}
}

void RenderSystem::SpriteBatchEnd()
{
	m_spriteBatch->End();
}

void RenderSystem::TrackEntity(
	ModelInstanceCache & modelInstances, 
	shared_ptr<world::WEM::RegionType> region,
	world::MaskType signature, 
	world::WorldEntityProxy<world::Model, world::Position> & entity, 
	Vector3 camera,
	bool moves,
	bool ignoreVerticalDistance)
{
	/*
	world::EntityID id = entity.GetID();
	auto it = modelInstances.find(region);
	if (it == modelInstances.end()) {
		it = modelInstances.insert(it, std::make_pair(region, std::map<world::MaskType, vector<RenderJob>>()));
	}
	auto & map = it->second;
	auto & position = entity.Get<world::Position>();
	auto & modelComp = entity.Get<world::Model>();
	float distance = 0.f;
	if (ignoreVerticalDistance) {
		distance = Vector2::Distance(Vector2(camera.x, camera.z), Vector2(position.Pos.x, position.Pos.z));
	}
	else {
		distance = Vector3::Distance(camera, position.Pos);
	}
	shared_ptr<Model> model = AssetManager::Get()->GetModel(modelComp.Asset, distance, position.Pos, modelComp.Type);
	if (map.find(signature) == map.end()) {
		map.insert(std::make_pair(signature, vector<RenderJob>()));
	}
	XMMATRIX translation = XMMatrixTranslation(position.Pos.x, position.Pos.y, position.Pos.z);
	XMMATRIX rotMat = XMMatrixRotationRollPitchYawFromVector(position.Rot);
	XMMATRIX world = XMMatrixMultiply(rotMat, translation);
	map[signature].push_back(RenderJob { moves,id,model,position.Pos,world });*/
}

void RenderSystem::TrackGridEntity(
	GridInstanceCache & gridInstances,
	shared_ptr<world::WEM::RegionType> region,
	world::MaskType signature,
	Vector3 position,
	Vector3 rotation,
	world::VoxelGridModel & gridComp,
	Vector3 camera,
	bool ignoreVerticalDistance
)
{
	world::EntityID id = gridComp.ID;
	auto it = gridInstances.find(region);
	if (it == gridInstances.end()) {
		it = gridInstances.insert(it, std::make_pair(region, std::map<world::MaskType, vector<RenderGridJob>>()));
	}
	auto & map = it->second;
	float distance = 0.f;
	if (ignoreVerticalDistance) {
		distance = Vector2::Distance(Vector2(camera.x, camera.z), Vector2(position.x, position.z));
	}
	else {
		distance = Vector3::Distance(camera, position);
	}
	if (map.find(signature) == map.end()) {
		map.insert(std::make_pair(signature, vector<RenderGridJob>()));
	}
	map[signature].push_back(RenderGridJob(gridComp,position,rotation));

}

void RenderSystem::UpdateVisibleRegions(Vector3 & cameraPosition, Vector3 & cameraRotation)
{
	m_visibleRegions.clear();
	int culledRegionCount = 0;
	for (auto & region : EM->GetRegions()) {
		if (IsRegionVisible(region, cameraPosition, cameraRotation, m_frustum)) {
			m_visibleRegions.insert(region);
		}
		else {
			culledRegionCount++;
		}
	}
	IEventManager::Invoke(EventTypes::GUI_DebugInfo, string("CullCount"), to_string(culledRegionCount));
}

bool RenderSystem::IsRectVisible(Rectangle & area, Vector2 & observerPos, Vector2 & fovNorm1, Vector2 & fovNorm2)
{
	if (area.Contains(observerPos)) return true;
	// for each corner in the area
	for (long x = area.x; x <= area.x + area.width; x += area.width) {
		for (long y = area.y; y <= area.y + area.height; y += area.height) {
			Vector2 direction((float)x - observerPos.x, (float)y - observerPos.y);
			if (direction.Dot(fovNorm1) > 0 && direction.Dot(fovNorm2) > 0) {
				// If any one of the corners is in the field of view of the observer, this area is visible
				return true;
			}
		}
	}
	return false;
}

bool RenderSystem::IsRegionVisible(shared_ptr<world::WEM::RegionType> region, Vector3 & position, Vector3 & rotation, BoundingFrustum & frustum)
{
	if (std::abs(rotation.z) <= 0.2f) {
		// 2D optimization when observer is mostly looking horizontally
		float halfFOV = m_fov * 0.5f;
		Vector2 fovNorm1(sinf(rotation.x + halfFOV), cosf(rotation.x + halfFOV));
		Vector2 fovNorm2(sinf(rotation.x - halfFOV), cosf(rotation.x - halfFOV));
		Vector2 observerPos(position.x, position.z);
		return IsRectVisible(region->GetArea(), observerPos, fovNorm1, fovNorm2);
	}
	else {
		// 3D frusum intersection test with region bounding box
		Vector2 areaCenter = region->GetArea().Center();
		BoundingBox box(Vector3(areaCenter.x, 0.f, areaCenter.y), Vector3(region->GetArea().width, 1000.f, region->GetArea().height));
		return frustum.Intersects(box);
	}
}

void RenderSystem::RenderModels(Vector3 & cameraPos, world::MaskType signatureMask,bool opaque)
{
	//auto movementMask = EM->GetMask<world::Movement>();
	//for (auto & visibleRegion : m_visibleRegions) {
	//	auto & regionCache = m_modelInstances[visibleRegion];
	//	for (auto & sigCache : regionCache) {
	//		world::MaskType signature = sigCache.first;
	//		if (signature == signatureMask) {
	//			for (auto & job : sigCache.second) {
	//				shared_ptr<Model> dxModel = job.model;
	//				//if (m_tracked.count(job.entity) != 0) {
	//					XMMATRIX & world = job.worldMatrix;
	//					if (job.moves) {
	//						auto & position = EM->GetEntity<world::Position>(job.entity)->Get<world::Position>();
	//						world::EntityInfo * info;


	//						XMMATRIX translation = XMMatrixTranslationFromVector(position.Pos);
	//						XMMATRIX rotMat = XMMatrixRotationRollPitchYawFromVector(position.Rot);
	//						world = XMMatrixMultiply(rotMat, translation);
	//					}

	//					RenderModel(dxModel, world, opaque);
	//					// collision box
	//					/*if ((signature & EM->GetMask<world::Collision>()) == EM->GetMask<world::Collision>()) {
	//						auto entity = EM->GetEntity<world::Position,world::Collision>(job.entity);
	//						auto & position = entity.Get<world::Position>();
	//						if (Vector3::DistanceSquared(position.Pos, cameraPos) < 100) {
	//							auto & collision = entity.Get<world::Collision>();
	//							auto box = DirectX::GeometricPrimitive::CreateBox(m_d3dContext.Get(), collision.BoundingBox.Size);
	//							XMMATRIX translation = XMMatrixTranslation(
	//								position.Pos.x,
	//								position.Pos.y,
	//								position.Pos.z
	//							);
	//							XMMATRIX rotMat = XMMatrixRotationRollPitchYawFromVector(position.Rot);
	//							XMMATRIX collisionWorld = XMMatrixMultiply(rotMat, translation);
	//							collisionWorld = XMMatrixMultiply(XMMatrixTranslationFromVector(collision.BoundingBox.Position), collisionWorld);
	//							auto color = Colors::White;
	//							switch (collision.Colliding) {
	//							case 1: color = Colors::Red; break;
	//							case 2: color = Colors::Orange; break;
	//							case 3: color = Colors::Green; break;
	//							}
	//							box->Draw(collisionWorld, m_viewMatrix, m_projMatrix, color, nullptr, true);
	//						}
	//					}*/
	//				//}
	//			}
	//		}
	//	}
	//}
}

void RenderSystem::RenderModel(shared_ptr<DirectX::Model> model,XMMATRIX world, bool alpha)
{
	if (model) {
		for (auto it = model->meshes.cbegin(); it != model->meshes.cend(); ++it)
		{
			auto mesh = it->get();
			assert(mesh != 0);

			mesh->PrepareForRendering(m_d3dContext.Get(), *m_states, alpha,m_wireframe);

			// Do model-level setCustomState work here

			mesh->Draw(m_d3dContext.Get(), world, m_viewMatrix, m_projMatrix, alpha);
		}
	}
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



void RenderSystem::UpdateEffectMatricies(std::shared_ptr<IEffectMatrices> effect, int backBufferWidth, int backBufferHeight)
{
	m_aspectRatio = float(backBufferWidth) / float(backBufferHeight);
	m_projMatrix = Matrix::CreatePerspectiveFieldOfView(m_fov,
		m_aspectRatio, m_clipNear,m_clipFar);
		
		
		effect->SetProjection(m_projMatrix);
		effect->SetWorld(m_worldMatrix);
		effect->SetView(m_viewMatrix);
}
void RenderSystem::CreateFromMatrixRH(BoundingFrustum& Out, CXMMATRIX Projection)
{
	// Corners of the projection frustum in homogenous space.
	static XMVECTORF32 HomogenousPoints[6] =
	{
	  { 1.0f,  0.0f, -1.0f, 1.0f },   // right (at far plane)
	  { -1.0f,  0.0f, -1.0f, 1.0f },   // left
	  { 0.0f,  1.0f, -1.0f, 1.0f },   // top
	  { 0.0f, -1.0f, -1.0f, 1.0f },   // bottom

	  { 0.0f, 0.0f, 1.0f, 1.0f },    // near
	  { 0.0f, 0.0f, 0.0f, 1.0f }     // far
	};

	XMVECTOR Determinant;
	XMMATRIX matInverse = XMMatrixInverse(&Determinant, Projection);

	// Compute the frustum corners in world space.
	XMVECTOR Points[6];

	for (size_t i = 0; i < 6; ++i)
	{
		// Transform point.
		Points[i] = XMVector4Transform(HomogenousPoints[i], matInverse);
	}

	Out.Origin = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Out.Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	// Compute the slopes.
	Points[0] = Points[0] * XMVectorReciprocal(XMVectorSplatZ(Points[0]));
	Points[1] = Points[1] * XMVectorReciprocal(XMVectorSplatZ(Points[1]));
	Points[2] = Points[2] * XMVectorReciprocal(XMVectorSplatZ(Points[2]));
	Points[3] = Points[3] * XMVectorReciprocal(XMVectorSplatZ(Points[3]));

	Out.RightSlope = XMVectorGetX(Points[0]);
	Out.LeftSlope = XMVectorGetX(Points[1]);
	Out.TopSlope = XMVectorGetY(Points[2]);
	Out.BottomSlope = XMVectorGetY(Points[3]);

	// Compute near and far.
	Points[4] = Points[4] * XMVectorReciprocal(XMVectorSplatW(Points[4]));
	Points[5] = Points[5] * XMVectorReciprocal(XMVectorSplatW(Points[5]));

	Out.Near = XMVectorGetZ(Points[4]);
	Out.Far = XMVectorGetZ(Points[5]);
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
	
	m_guiSystem->UpdateUI(backBufferWidth, backBufferHeight);
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
	AssetManager::Get()->SetDevice(m_d3dDevice);
	AssetManager::Get()->CreateEffects(m_d3dContext);

	
	
	m_effectOrder = vector<string>{ "Terrain","Wood", "Water","Test","Default" };

	m_spriteBatch = std::make_unique<SpriteBatch>(m_d3dContext.Get());

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

}

DirectX::XMMATRIX RenderSystem::GetViewMatrix()
{
	auto player = EM->GetEntity<world::Position, world::Player>(EM->PlayerID());
	world::Position & position = player->Get<world::Position>();
	world::Player & playerComp = player->Get<world::Player>();
	float y = sinf(playerComp.CameraPitch);
	float r = cosf(playerComp.CameraPitch);
	float z = r * cosf(position.Rot.y);
	float x = r * sinf(position.Rot.y);
	
	Vector3 cameraPos; 
	Vector3 lookAt; 

	switch (playerComp.InteractionState) {
	case world::Player::InteractionStates::FirstPerson:
		cameraPos = position.Pos + Vector3(0.f, 1.7f, 0.f);
		lookAt = cameraPos + SimpleMath::Vector3(x, y, z);
		break;
	case world::Player::InteractionStates::ThirdPerson:
		lookAt = position.Pos + Vector3(0.f, 1.7f, 0.f);
		cameraPos = lookAt - Vector3(x, y, z) * 3.f;
		break;
	}
	//Vector3 lookAt = position.Pos + Vector3(0.f,1.7f,0.f); // third person
	// XMMATRIX view = XMMatrixLookAtRH(position.Pos, lookAt, SimpleMath::Vector3(0.f, 1.f, 0.f)); // first person
	//XMVECTOR cameraPos = lookAt - Vector3(x, y, z) * 3.f; // third person

	XMMATRIX view = XMMatrixLookAtRH(cameraPos, lookAt, SimpleMath::Vector3(0.f, 1.f, 0.f));
	m_viewMatrix = view;
	XMMATRIX ProjectionMatrixLH = XMMatrixPerspectiveFovLH(m_fov,  m_aspectRatio, m_clipNear, m_clipFar);
	XMMATRIX ViewMatrixLH = XMMatrixLookAtLH(cameraPos, lookAt, SimpleMath::Vector3(0.f, 1.f, 0.f));
	XMVECTOR Determinant;
	XMMATRIX InvViewMatrixLH = XMMatrixInverse(&Determinant, ViewMatrixLH);
	m_frustum = BoundingFrustum();
	m_frustum.CreateFromMatrix(m_frustum, ProjectionMatrixLH);
	m_frustum.Transform(m_frustum, InvViewMatrixLH);
	//CreateFromMatrixRH(m_frustum,  m_viewMatrix);
	return view;
}


void RenderSystem::SetStates()
{
	m_states = std::make_unique<CommonStates>(m_d3dDevice.Get());
	auto sampler = m_states->LinearWrap();
	m_d3dContext->PSSetSamplers(0, 1, &sampler);
	m_d3dContext->RSSetState(m_states->CullCounterClockwise());
	float alpha[4] = { 0.f,0.f, 0.f, 1.f };
	m_d3dContext->OMSetBlendState(m_states->NonPremultiplied(), nullptr, 0xFFFFFFFF);
	m_d3dContext->OMSetDepthStencilState(m_states->DepthDefault(), 0);
}

void RenderSystem::RenderVBO(shared_ptr<Components::PositionNormalTextureTangentColorVBO> vbo)
{
	// make sure the buffers have been updated
	if (vbo->LODchanged && vbo->Vertices.size() != 0) {
		vbo->CreateBuffers(m_d3dDevice);
		vbo->LODchanged = false;
	}
	if (vbo->Vertices.size() > 0) {
		// Set vertex buffer stride and offset.
		UINT stride = sizeof(VertexPositionNormalTangentColorTexture);
		UINT offset = 0;

		// Set the vertex buffer to active in the input assembler so it can be rendered.
		m_d3dContext->IASetVertexBuffers(0, 1, vbo->VB.GetAddressOf(), &stride, &offset);

		// Set the index buffer to active in the input assembler so it can be rendered.
		m_d3dContext->IASetIndexBuffer(vbo->IB.Get(), DXGI_FORMAT_R16_UINT, 0);

		// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
		m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Draw
		m_d3dContext->DrawIndexed(vbo->Indices.size(), 0, 0);
	}
	
}

void RenderSystem::RenderTerrain(shared_ptr<world::WEM::RegionType> region, bool alpha)
{
	Vector2 location = region->GetCenter();
	Vector3 position = Vector3::Zero;
	RenderModel(SM->GetSystem<world::TerrainSystem>("Terrain")->GetModel(region), Matrix::CreateTranslation(position), alpha);
}

void RenderSystem::LoadRegion(shared_ptr<world::WEM::RegionType> region)
{
	const auto excludeMask = EM->GetMask<world::Movement>();
	//----------------------------------------------------------------
	// Static models
	world::MaskType modelMask = EM->GetMask<world::Position, world::Model>();
	TaskManager::Get().Push(Task([=] {
		world::EntityCache<world::Position, world::Model> staticModels;
		region->LoadEntities(staticModels, [=](world::MaskType sig) {
			return !(sig & excludeMask) && (sig & modelMask) == modelMask;
		});

		for (auto & entity : staticModels) {
			auto & position = entity.Get<world::Position>();
			auto & model = entity.Get<world::Model>();

			CreateJob(m_jobs[region].opaque, entity.GetID(), position.Pos, position.Rot, model.Asset, model.Type);
			bool alpha = false;
			try {
				AssetManager::Get()->GetModel(model.Asset, model.Type)->IsAlpha();
			}
			catch (std::exception & ex) {
			}
			if (alpha) CreateJob(m_jobs[region].alpha, entity.GetID(), position.Pos, position.Rot, model.Asset, model.Type);
		}
	}, modelMask, modelMask));
	//----------------------------------------------------------------
	// Grids

	world::MaskType gridMask = EM->GetMask<world::Position, world::VoxelGridModel>();
	TaskManager::Get().Push(Task([=] {
		std::lock_guard<std::mutex> guard(m_mutex);
		world::EntityCache<world::Position, world::VoxelGridModel> grids;
		region->LoadEntities(grids, [=](world::MaskType sig) {
			return !(sig & excludeMask) && (sig & gridMask) == gridMask;
		});
		for (auto & entity : grids) {
			auto & position = entity.Get<world::Position>();
			auto & gridModel = entity.Get<world::VoxelGridModel>();
			
			m_gridJobs[region].push_back(RenderGridJob(gridModel, position.Pos, position.Rot));
		}
	}, gridMask, gridMask));
}

void RenderSystem::UnloadRegion(shared_ptr<world::WEM::RegionType> region)
{
	// clear caches
	m_jobs[region].Clear();
}

void RenderSystem::InitializeJobCache()
{
	for (auto & region : EM->GetRegions()) {
		m_jobs.insert(std::make_pair(region, RegionJobCache()));
	}
}

void RenderSystem::CreateJob(
	vector<RenderJob> & jobs, 
	world::EntityID entity,
	Vector3 position, 
	Vector3 rotation, 
	AssetID modelAsset,
	AssetType assetType
) {
	std::lock_guard<std::mutex> guard(m_mutex);
	jobs.push_back(RenderJob(entity, position, rotation, modelAsset,assetType));
}

RenderSystem::JobCaches RenderSystem::CreateDynamicJobs()
{
	JobCaches caches;

	auto dynamicModels = EM->NewEntityCache<world::Position, world::Movement, world::Model>();
	world::MaskType dynamicMask = dynamicModels.GetComponentMask();
	EM->UpdateCache(dynamicModels);
	
	for (auto & regionCache : dynamicModels.GetCaches()) {
		if (m_visibleRegions.count(regionCache.first)) {
			for (auto & entity : regionCache.second) {
				auto & position = entity.Get<world::Position>();
				auto & model = entity.Get<world::Model>();
				
				RegionJobCache cache;
				CreateJob(cache.opaque, entity.GetID(), position.Pos, position.Rot, model.Asset, model.Type);
				bool alpha = AssetManager::Get()->GetModel(model.Asset,model.Type)->IsAlpha();
				if (alpha) CreateJob(cache.alpha, entity.GetID(), position.Pos, position.Rot, model.Asset, model.Type);
				caches.insert(std::make_pair(regionCache.first, cache));
			}
		}
	}
	return caches;
}

RenderSystem::JobCaches RenderSystem::CreateGridJobs(Vector3 & center, BoundingFrustum & frustum)
{
	JobCaches caches;
	for (auto & pair : m_gridJobs) {
		for (auto & gridJob : pair.second) {
			BoundingFrustum gridRelativeFrustum;
			Matrix inverse = Matrix(gridJob.worldMatrix).Invert();
			frustum.Transform(gridRelativeFrustum, inverse);
			// Cull voxels by the frustum
			auto && intersection = gridJob.voxels.GetIntersection(gridRelativeFrustum);
			vector<RenderJobVoxel> voxels = vector<RenderJobVoxel>(intersection.begin(), intersection.end());
			// Depth sort voxels
			//DepthSort(voxels, center);
			for (auto & voxel : voxels) {
				caches[pair.first].Add(voxel.jobs);
			}
		}
	}
	return caches;
}

void RenderSystem::DepthSort(vector<shared_ptr<world::WEM::RegionType>>& regions,Vector2 center)
{
	std::sort(regions.begin(), regions.end(), [=](shared_ptr<world::WEM::RegionType> & a, shared_ptr<world::WEM::RegionType> & b) {
		return Vector2::DistanceSquared(a->GetCenter(), center) < Vector2::DistanceSquared(b->GetCenter(), center);
	});
}

void RenderSystem::DepthSort(vector<RenderJob>& jobs, Vector3 center)
{
	std::sort(jobs.begin(), jobs.end(), [=](RenderJob & a, RenderJob & b) {
		return Vector3::DistanceSquared(a.position, center) < Vector3::DistanceSquared(b.position, center);
	});
}
void RenderSystem::DepthSort(vector<RenderJobVoxel> & voxels, Vector3 center) {
	std::sort(voxels.begin(), voxels.end(), [=](RenderJobVoxel & a, RenderJobVoxel & b) {
		return Vector3::DistanceSquared(a.GetPosition(), center) < Vector3::DistanceSquared(b.GetPosition(), center);
	});
}

void RenderSystem::RenderJobs(vector<RenderJob>& jobs,Vector3 cameraPos,bool alpha)
{
	if (alpha) {
		DepthSort(jobs, cameraPos);
		for (auto jobIt = jobs.rbegin(); jobIt != jobs.rend(); jobIt++) {
			RenderRenderJob(*jobIt,cameraPos,alpha);
		}
	}
	else {
		for (auto & job : jobs) {
			RenderRenderJob(job, cameraPos, alpha);
		}
	}
	
	
}

void RenderSystem::RenderRenderJob(RenderJob & job, Vector3 cameraPos, bool alpha)
{
	// Get the current LOD
	auto cmf = AssetManager::Get()->GetModel(job.modelAsset, job.assetType);
	if (cmf) {
		shared_ptr<DirectX::Model> dxModel = cmf->GetModel(m_d3dDevice.Get(), AssetManager::Get()->GetFxFactory(), Vector3::Distance(job.position, cameraPos));

		if (dxModel != nullptr) RenderModel(dxModel, job.worldMatrix, alpha);
	}
}


string RenderSystem::Name()
{
	return "Render";
}

void RenderSystem::UpdateFramerate()
{
	static std::chrono::milliseconds last;
	auto now = std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::system_clock::now().time_since_epoch()
		);
	m_frameDeltas.push_back((now - last).count());
	m_frameDeltas.pop_front();
	int avg = 0;
	for (int delta : m_frameDeltas) {
		avg += delta;
	}
	avg /= (int)m_frameDeltas.size();
	avg = (int)(1.0 / ((double)avg / 1000));
	// convert average milliseconds to frames per second
	IEventManager::Invoke(EventTypes::GUI_DebugInfo,string("Framerate"),to_string(avg));
	last = now;
}
