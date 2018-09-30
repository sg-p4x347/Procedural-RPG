#include "pch.h"
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
SM(systemManager)
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
	//m_scissorState.reset(scissorState);
		// error
	//SetFont("impact");
	for (int i = 0; i < 20; i++) {
		m_frameDeltas.push_back(0);
	}
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::Update(double & elapsed)
{
	//https://stackoverflow.com/questions/17655442/how-can-i-repeat-my-texture-in-dx

	if (EM) GetViewMatrix();
	// Clear the screen
	Clear();
	// Render all Vertex Buffer Objects
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
	//for (string & effectName : m_effectOrder) {
	//	// Bind this effect
	//	shared_ptr<IEffect> effect;
	//	//if (AssetManager::Get()->GetEffect<DGSLEffect>(effectName, effect)) {
	//	//shared_ptr<IEffect> effect;
	//	if (AssetManager::Get()->GetEffect<IEffect>(effectName, effect)) {
	//		// camera
	//		shared_ptr<IEffectMatrices> iEffectMatricies = dynamic_pointer_cast<IEffectMatrices>(effect);
	//		if (iEffectMatricies) {
	//			UpdateEffectMatricies(iEffectMatricies, m_outputWidth, m_outputHeight);
	//			
	//			//effect->SetViewport(float(m_outputWidth), float(m_outputHeight));
	//		}
	//		
	//		effect->Apply(m_d3dContext.Get());
	//		// Input Layout
	//		m_d3dContext->IASetInputLayout(AssetManager::Get()->GetInputLayout(effectName).Get());
	//		/*if (effectName == "Water") {
	//			m_d3dContext->IASetInputLayout(m_waterLayout.Get());
	//		}
	//		else {
	//			m_d3dContext->IASetInputLayout(m_inputLayout.Get());
	//		}*/
	//		// Render VBOs with the effect
	//		if (m_mutex.try_lock()) {
	//			if (m_VBOs.find(effectName) != m_VBOs.end()) {
	//				for (auto & vbo : m_VBOs[effectName]) {
	//					RenderVBO(vbo);
	//				}
	//			}
	//			m_mutex.unlock();
	//		}
	//	}

	//	// Render all Components::Model with the effect
	//	//if (m_Models.find(effectName) != m_Models.end()) {
	//	//	for (auto & model : m_Models[effectName]) {
	//	//		EntityPtr entity;
	//	//		if (EM->Find(model->ID, entity)) {
	//	//			auto position = entity->GetComponent<Components::Position>("Position");
	//	//			auto dxModel = AssetManager::Get()->GetModel(model->Path,Vector3::Distance(EM->PlayerPos()->Pos,position->Pos),model->Procedural);
	//	//			
	//	//			RenderModel(dxModel,effect, position->Pos, position->Rot, false/*model->BackfaceCulling*/);
	//	//			
	//	//		}
	//	//	}
	//	//}
	//	
	//}
	m_mutex.lock();
	for (auto & instances : m_modelInstances) {
		shared_ptr<Model> dxModel = instances.first;
		for (auto & job : instances.second) {
			if (m_tracked.count(job.entity) != 0)
				RenderModel(dxModel, job.worldMatrix, true);
		}
	}
	m_mutex.unlock();
	/*for (auto & instances : m_modelInstances) {
		shared_ptr<Model> dxModel = instances.first;
		for (auto & job : instances.second) {
			if (m_tracked.count(job.entity) != 0)
				RenderModel(dxModel, job.worldMatrix, false);
		}
	}*/
	// Render all buildings
	/*for (EntityPtr & building : EM->FindEntities("Building")) {
			
		auto position = building->GetComponent<Components::Position>("Position");
		auto dxModel = SM->GetSystem<BuildingSystem>("Building")->GetModel(building, Vector3::Distance(EM->PlayerPos()->Pos, position->Pos));

		RenderCompositeModel(dxModel, position->Pos, position->Rot, true);
	}*/
}
//void RenderSystem::SyncEntities()
//{
//	//std::thread([=] {
//	if (EM && EM->Player()) {
//		std::map<string, vector<shared_ptr<Components::PositionNormalTextureTangentColorVBO>>> vbos;
//		for (auto & entity : EM->FindEntities(m_VBOmask)) {
//			shared_ptr<Components::PositionNormalTextureTangentColorVBO> vbo = entity->GetComponent<Components::PositionNormalTextureTangentColorVBO>("PositionNormalTextureTangentColorVBO");
//			if (vbos.find(vbo->Effect) == vbos.end()) {
//				vbos.insert(std::pair<string, vector<shared_ptr<Components::PositionNormalTextureTangentColorVBO>>>(vbo->Effect, vector<shared_ptr<Components::PositionNormalTextureTangentColorVBO>>()));
//			}
//			vbos[vbo->Effect].push_back(vbo);
//		}
//		m_mutex.lock();
//		m_VBOs = vbos;
//		m_mutex.unlock();
//		std::map<string, vector<shared_ptr<Components::Model>>> models;
//		for (auto & entity : EM->FindEntitiesInRange(m_ModelMask, EM->PlayerPos()->Pos, 64)) {
//			//for (auto & entity : EM->FindEntities(m_ModelMask)) {
//			shared_ptr<Components::Model> model = entity->GetComponent<Components::Model>("Model");
//			if (models.find(model->Effect) == models.end()) {
//				models.insert(std::pair<string, vector<shared_ptr<Components::Model>>>(model->Effect, vector<shared_ptr<Components::Model>>()));
//			}
//			models[model->Effect].push_back(model);
//		}
//		m_mutex.lock();
//		m_Models = models;
//		m_mutex.unlock();
//
//		//}).detach();
//	}
//}
void RenderSystem::SyncEntities()
{
	//std::thread([=] {
	if (EM) {
		/*std::map<string, vector<shared_ptr<Components::PositionNormalTextureTangentColorVBO>>> vbos;
		for (auto & entity : EM->FindEntities(m_VBOmask)) {
			shared_ptr<Components::PositionNormalTextureTangentColorVBO> vbo = entity->GetComponent<Components::PositionNormalTextureTangentColorVBO>("PositionNormalTextureTangentColorVBO");
			if (vbos.find(vbo->Effect) == vbos.end()) {
				vbos.insert(std::pair<string, vector<shared_ptr<Components::PositionNormalTextureTangentColorVBO>>>(vbo->Effect, vector<shared_ptr<Components::PositionNormalTextureTangentColorVBO>>()));
			}
			vbos[vbo->Effect].push_back(vbo);
		}
		m_mutex.lock();
		m_VBOs = vbos;
		m_mutex.unlock();*/
		
		//std::map<shared_ptr<Model>, vector<XMMATRIX>> modelInstances;
		//std::set<EntityPtr> tracked;
		

		/*auto modelEntities = EM->NewEntityCache<world::Model, world::Position>();
		EM->UpdateCache(modelEntities);

		for (auto & entity : modelEntities) {
			TrackEntity(m_modelInstances,m_tracked,entity.GetProxy());
		}*/
		

		// terrain
		world::MaskType accessMask = EM->GetMask<world::Terrain, world::Model, world::Position>();
		TaskManager::Get().Push(Task([&]() {
			m_syncMutex.lock();
			m_modelInstancesTemp.clear();
			m_trackedTemp.clear();
				auto terrainEntities = EM->NewEntityCache<world::Terrain, world::Model, world::Position>();
				EM->UpdateGlobalCache(terrainEntities);

				for (auto & entity : terrainEntities) {
					auto modelEntity = EM->GetEntity<world::Model, world::Position>(entity.GetID());
					TrackEntity(m_modelInstancesTemp, m_trackedTemp, modelEntity,true);
				}
				m_mutex.lock();
				std::swap(m_modelInstancesTemp, m_modelInstances);
				std::swap(m_trackedTemp, m_tracked);
				m_mutex.unlock();
				m_syncMutex.unlock();
		}, 
			accessMask,
			accessMask));
		
		
		//m_mutex.lock();
		//m_modelInstances = modelInstances;
		//m_tracked = tracked;
		//m_mutex.unlock();
		
		//m_models = models;
		

		
	}
	//}).detach();
}
void RenderSystem::SetViewport(int width, int height)
{
	m_outputWidth = std::max(width, 1);
	m_outputHeight = std::max(height, 1);
	m_projMatrix = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(70.f),
		float(width) / float(height), 0.1f, 8000.f);
	CreateResources();
}

Rectangle RenderSystem::GetViewport()
{
	return Rectangle(0,0, m_outputWidth,m_outputHeight);
}

void RenderSystem::InitializeWorldRendering(world::WEM * entityManager)
{
	EM = entityManager;
	if (EM) {
		RegisterHandlers();
		SyncEntities();
	}
}

void RenderSystem::RegisterHandlers()
{
	IEventManager::RegisterHandler(EventTypes::WEM_Resync, std::function<void(void)>([=]() {
		if (EM) {
			SyncEntities();
		}
	}));
	IEventManager::RegisterHandler(EventTypes::Entity_Deleted, std::function<void(EntityPtr)>([&](EntityPtr entity) {
		/*if (entity->HasComponents(EM->ComponentMask("Model"))) {
			m_tracked.erase(entity);
		}*/
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
	auto spriteFont = GetFont(font, size);
	if (spriteFont) {
		spriteFont->DrawString(m_spriteBatch.get(), ansi2unicode(text).c_str(), position, color,0.f,Vector2::Zero,Vector2::One,DirectX::SpriteEffects::SpriteEffects_None,layer);
	}
}

void RenderSystem::SpriteBatchEnd()
{
	m_spriteBatch->End();
}

void RenderSystem::TrackEntity(std::map<shared_ptr<Model>, vector<RenderEntityJob>> & modelInstances, std::set<world::EntityID> & tracked, world::WorldEntityProxy<world::Model, world::Position> & entity,bool ignoreVerticalDistance)
{
	world::EntityID id = entity.GetID();
	if (tracked.count(id) == 0) {
		auto & position = entity.Get<world::Position>();
		auto & modelComp = entity.Get<world::Model>();
		float distance = 0.f;
		if (ignoreVerticalDistance) {
			auto pos = EM->PlayerPos();
			distance = Vector2::Distance(Vector2(pos.x, pos.z), Vector2(position.Pos.x, position.Pos.z));
		}
		else {
			distance = Vector3::Distance(EM->PlayerPos(), position.Pos);
		}
		shared_ptr<Model> model = AssetManager::Get()->GetModel(modelComp.Asset, distance, position.Pos, modelComp.Type);
		if (modelInstances.find(model) == modelInstances.end()) {
			modelInstances.insert(std::make_pair(model, vector<RenderEntityJob>()));
		}
		XMMATRIX translation = XMMatrixTranslation(position.Pos.x, position.Pos.y, position.Pos.z);
		XMMATRIX rotMat = XMMatrixRotationRollPitchYawFromVector(position.Rot);
		XMMATRIX world = XMMatrixMultiply(rotMat, translation);
		modelInstances[model].push_back(RenderEntityJob { id,world });
		tracked.insert(id);
	}
}

void RenderSystem::RenderModel(shared_ptr<DirectX::Model> model,XMMATRIX world, bool opaque)
{
	if (opaque) {
		for (auto it = model->meshes.cbegin(); it != model->meshes.cend(); ++it)
		{
			auto mesh = it->get();
			assert(mesh != 0);

			mesh->PrepareForRendering(m_d3dContext.Get(), *m_states, false);

			// Do model-level setCustomState work here

			mesh->Draw(m_d3dContext.Get(), world, m_viewMatrix, m_projMatrix, false);
		}
	}
	else {
		for (auto it = model->meshes.cbegin(); it != model->meshes.cend(); ++it)
		{
			auto mesh = it->get();
			assert(mesh != 0);

			mesh->PrepareForRendering(m_d3dContext.Get(), *m_states, true);

			// Do model-level setCustomState work here

			mesh->Draw(m_d3dContext.Get(), world, m_viewMatrix, m_projMatrix, true);
		}
	}
	//model->Draw(m_d3dContext.Get(), *m_states, world, m_viewMatrix, m_projMatrix);


	// An example of using a single custom effect when drawing all the parts of a Model
	//if (effect) {
	//	// Creating input layouts is expensive, so it shouldn't be done every frame
	//	//std::vector<Microsoft::WRL::ComPtr<ID3D11InputLayout>> newInputLayouts;
	//	bool effectsChanged = false;
	//	for (auto mit = model->meshes.cbegin(); mit != model->meshes.cend(); ++mit)
	//	{
	//		auto mesh = mit->get();
	//		assert(mesh != 0);
	//		
	//		for (auto it = mesh->meshParts.cbegin(); it != mesh->meshParts.cend(); ++it)
	//		{
	//			auto part = it->get();
	//			assert(part != 0);
	//			// Change the effect
	//			if (part->effect != effect) {
	//				
	//				part->effect = effect;
	//				part->ModifyEffect(m_d3dDevice.Get(), effect, false);
	//				/*Microsoft::WRL::ComPtr<ID3D11InputLayout> il;
	//				part->CreateInputLayout(m_d3dDevice.Get(), effect.get(), il.GetAddressOf());
	//				part->inputLayout = il;*/
	//				//newInputLayouts.emplace_back(il);
	//				effectsChanged = true;
	//			}
	//		}
	//	}
	//	if (effectsChanged) model->Modified();

	//	// Draw Model with custom effect override
	//	auto imatrices = dynamic_cast<IEffectMatrices*>(effect.get());
	//	if (imatrices)
	//	{
	//		imatrices->SetWorld(world);
	//		imatrices->SetView(m_viewMatrix);
	//		imatrices->SetProjection(m_projMatrix);
	//	}

	//	//size_t count = 0;
	//	//for (auto mit = model->meshes.cbegin(); mit != model->meshes.cend(); ++mit)
	//	//{
	//	//	auto mesh = mit->get();
	//	//	assert(mesh != 0);

	//	//	for (auto it = mesh->meshParts.cbegin(); it != mesh->meshParts.cend(); ++it)
	//	//	{
	//	//		auto part = it->get();
	//	//		assert(part != 0);

	//	//		// Could call if a custom transformation was desired for each part
	//	//		// if (imatricies) imatrices->SetWorld( local ) 

	//	//		/*part->Draw(m_d3dContext.Get(), effect,
	//	//			newInputLayouts[count++].Get());*/
	//	//		part->Draw(m_d3dContext.Get(), effect.get(),
	//	//			part->inputLayout.Get());
	//	//	}
	//	//}
	//}
	
	//else {
	/*	for (auto it = model->meshes.cbegin(); it != model->meshes.cend(); ++it)
		{
			auto mesh = it->get();
			assert(mesh != 0);
			
			RenderModelMesh(mesh, world);
			
		}*/
	//}
	
	//model->Draw(m_d3dContext.Get(), *m_states, final, m_viewMatrix, m_projMatrix);
}

void RenderSystem::RenderCompositeModel(shared_ptr<CompositeModel> model, Vector3 & position, Vector3 & rotation, bool backfaceCulling)
{
	XMMATRIX translation = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotMat = XMMatrixRotationRollPitchYawFromVector(rotation);
	XMMATRIX world = XMMatrixMultiply(rotMat, translation);

	for (auto & pair : model->meshes) {
		RenderModelMesh(pair.first.get(), XMMatrixMultiply(pair.second,world), backfaceCulling);
	}

}

void RenderSystem::RenderModelMesh(DirectX::ModelMesh * mesh, XMMATRIX world, bool backfaceCulling)
{
	mesh->PrepareForRendering(m_d3dContext.Get(), *m_states, false);
	if (!backfaceCulling) {
		m_d3dContext->RSSetState(m_states->CullNone());
	}
	else {
		m_d3dContext->RSSetState(m_states->CullCounterClockwise());

	}
	
	//// Do model-level setCustomState work here
	//m_d3dContext->RSSetState(m_states->CullNone());
	mesh->Draw(m_d3dContext.Get(), world, m_viewMatrix, m_projMatrix, false);
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
	m_projMatrix = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(70.f),
		float(backBufferWidth) / float(backBufferHeight), 0.1f, 8000.f);
		/*DGSLEffectFactory::DGSLEffectInfo info;
	    wchar_t* wide = nullptr;
		Utility::StringToWchar(effectKey, wide);
		info.name = wide;
		shared_ptr<DGSLEffect> effect = std::dynamic_pointer_cast<DGSLEffect>(m_fxFactory->CreateEffect(info, m_d3dContext.Get()));
		*/
		
		
		effect->SetProjection(m_projMatrix);
		effect->SetWorld(m_worldMatrix);
		effect->SetView(m_viewMatrix);
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
	world::Position & position = EM->GetEntity<world::Position>(EM->PlayerID()).Get<world::Position>();
	float y = sinf(position.Rot.y);
	float r = cosf(position.Rot.y);
	float z = r*cosf(position.Rot.x);
	float x = r*sinf(position.Rot.x);

	XMVECTOR lookAt = position.Pos + SimpleMath::Vector3(x, y, z);

	XMMATRIX view = XMMatrixLookAtRH(position.Pos, lookAt, SimpleMath::Vector3(0.f, 1.f, 0.f));
	m_viewMatrix = view;
	
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
	avg /= m_frameDeltas.size();
	avg = 1.0 / ((double)avg / 1000);
	// convert average milliseconds to frames per second
	m_guiSystem->SetTextByID("Framerate","FPS: " + to_string(avg));
	last = now;
}
