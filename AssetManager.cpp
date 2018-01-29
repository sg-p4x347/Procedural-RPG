#include "pch.h"
#include "AssetManager.h"
#include "ModelAsset.h"
AssetManager * AssetManager::m_instance = nullptr;
void AssetManager::CreateEffects(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	//----------------------------------------------------------------
	// Terrain Effect
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	m_pixelShaders.insert(std::pair<string, Microsoft::WRL::ComPtr<ID3D11PixelShader>>("Terrain", pixelShader));
	auto blob = DX::ReadData(L"Assets\\Terrain.cso"); // .cso is the compiled version of the hlsl shader (compiled shader object)
	DX::ThrowIfFailed(m_d3dDevice->CreatePixelShader(&blob.front(), blob.size(),
		nullptr, pixelShader.ReleaseAndGetAddressOf()));

	shared_ptr<DGSLEffect> terrain = std::make_shared<DGSLEffect>(m_d3dDevice.Get(), pixelShader.Get());

	terrain->SetTexture(AssetManager::Get()->GetTexture("dirt").Get());
	terrain->SetTexture(1, AssetManager::Get()->GetTexture("grass").Get());
	terrain->SetTexture(2, AssetManager::Get()->GetTexture("stone").Get());
	terrain->SetTexture(3, AssetManager::Get()->GetTexture("snow").Get());

	terrain->SetTextureEnabled(true);
	terrain->SetVertexColorEnabled(true);

	terrain->EnableDefaultLighting();

	//----------------------------------------------------------------
	// Water Effect

	DGSLEffectFactory::DGSLEffectInfo info;
	info.name = L"Water";
	info.pixelShader = L"Assets\\Water.cso";
	shared_ptr<DGSLEffect> water = std::dynamic_pointer_cast <DGSLEffect>(m_DGSLfxFactory->CreateDGSLEffect(info, context.Get()));
	water->SetTexture(AssetManager::Get()->GetTexture("water").Get());
	water->SetTextureEnabled(true);
	water->SetVertexColorEnabled(true);
	water->EnableDefaultLighting();
	


	AddEffect("Terrain", terrain);
	AddEffect("Water", water);
}
AssetEntityManager * AssetManager::GetProceduralEM()
{
	return m_proceduralEM.get();
}
AssetManager::AssetManager() : m_fontSize(32)
{
}

Filesystem::path AssetManager::FullPath(string path, bool procedural, string extension)
{
	return Filesystem::path((procedural ? m_proceduralDir : m_authoredDir) / (path + extension));
}

Filesystem::path AssetManager::AppendPath(string path, string type)
{
	return Filesystem::path( m_authoredDir / type / path);
}



AssetManager * AssetManager::Get()
{
	if (!m_instance) {
		m_instance = new AssetManager();
	}
	return m_instance;
}

void AssetManager::SetAssetDir(Filesystem::path assets)
{
	m_authoredDir = assets;
	m_authoredEM.reset(new AssetEntityManager(assets));
}

void AssetManager::SetProceduralAssetDir(Filesystem::path procedural)
{
	m_proceduralDir = procedural;
	m_vboParser = std::make_unique<VboParser>(procedural);
	m_proceduralEM.reset(new AssetEntityManager(procedural));
}

void AssetManager::CleanupProceduralAssets()
{
	m_proceduralEM.reset(nullptr);
}

void AssetManager::SetDevice(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	m_d3dDevice = device;
	m_DGSLfxFactory = std::make_unique<DGSLEffectFactory>(m_d3dDevice.Get());
	m_DGSLfxFactory->SetDirectory(L".\\Assets\\");

	m_fxFactory = std::make_unique<EffectFactory>(m_d3dDevice.Get());
	m_fxFactory->SetDirectory(L".\\Assets\\");
}

shared_ptr<SpriteFont> AssetManager::GetFont(string path)
{
	try {

		// Search cache
		if (m_fonts.find(path) != m_fonts.end()) {
			return m_fonts[path];
		}
		if (m_d3dDevice == nullptr) throw std::exception("AssetManager device not set");
		// get the path
		Filesystem::path fullPath = FullPath(path, false, ".spritefont");
		// Load from file
		shared_ptr<SpriteFont> font = std::make_shared<SpriteFont>(m_d3dDevice.Get(),fullPath.c_str());
		m_fonts.insert(std::pair<string, shared_ptr<SpriteFont>>(path, font));
		return font;
	}
	catch (std::exception ex) {
		Utility::OutputException(path + ' ' + ex.what());
	}
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> AssetManager::GetTexture(string path,bool procedural)
{
	try {
		
		// Search cache
		if (m_textures.find(path) != m_textures.end()) {
			return m_textures[path];
		}
		if (m_d3dDevice == nullptr) throw std::exception("AssetManager device not set");
		// get the path
		Filesystem::path fullPath = FullPath(path, procedural, ".dds");
		// Load from file
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
		DX::ThrowIfFailed(
			DirectX::CreateDDSTextureFromFile(m_d3dDevice.Get(), fullPath.c_str(), nullptr,
				texture.ReleaseAndGetAddressOf())
		);
		m_textures.insert(std::pair<string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>(path, texture));
		return texture;
	}
	catch (std::exception ex) {
		Utility::OutputException(path + ' ' + ex.what());
	}
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> AssetManager::GetWicTexture(string path)
{
	try {

		// Search cache
		if (m_WicTextures.find(path) != m_WicTextures.end()) {
			return m_WicTextures[path];
		}
		if (m_d3dDevice == nullptr) throw std::exception("AssetManager device not set");
		// get the path
		Filesystem::path fullPath = AppendPath(path,"Textures");
		// Load from file
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
		DX::ThrowIfFailed(
			DirectX::CreateWICTextureFromFile(m_d3dDevice.Get(), fullPath.c_str(), nullptr,
				texture.ReleaseAndGetAddressOf())
		);
		m_WicTextures.insert(std::pair<string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>(path, texture));
		return texture;
	}
	catch (std::exception ex) {
		Utility::OutputException(path + ' ' + ex.what());
	}
}

std::shared_ptr<Model> AssetManager::GetModel(string path, float distance, bool procedural)
{
	try {
		if (m_d3dDevice == nullptr) throw std::exception("AssetManager device not set");
		// get the entity
		shared_ptr<ModelAsset> modelAsset = procedural ? m_proceduralEM->GetModel(path) : m_authoredEM->GetModel(path);
		// get the LOD level
		int lod = std::min(modelAsset->LodCount-1,(int)distance / modelAsset->LodSpacing);
		// Get the model
		std::shared_ptr<Model> model;
		if (modelAsset->LODs.size() <= lod || !modelAsset->LODs[lod]) {
			//----------------------------------------------------------------
			// Cache the LOD

			// get the path
			Filesystem::path fullPath = FullPath(path + '_' + std::to_string(lod), procedural, procedural ? ".vbo" : ".cmo");
			// Load from file
			if (procedural) {
				model.reset(Model::CreateFromVBO(m_d3dDevice.Get(), fullPath.c_str()).release());
			}
			else {
				model.reset(Model::CreateFromCMO(m_d3dDevice.Get(), fullPath.c_str(), *m_fxFactory).release());
			}
			if (modelAsset->LODs.size() <= lod) {
				modelAsset->LODs.resize(lod + 1);
			}
			modelAsset->LODs.insert(modelAsset->LODs.begin() + lod, model);
		}
		else {
			model = modelAsset->LODs[lod];
		}
		return model;
		//model->UpdateEffects([=](IEffect* effect)
		//{
		//	auto basic = dynamic_cast<BasicEffect*>(effect);
		//	if (basic)
		//	{
		//		basic->SetAlpha(0.5);
		//		/*basic->SetTextureEnabled(true);
		//		basic->SetTexture(tex.Get());
		//		basic->EnableDefaultLighting();*/
		//	}
		//});
		
	}
	catch (std::exception ex) {
		Utility::OutputException(path + ' ' + ex.what());
	}
}
int AssetManager::GetFontSize()
{
	return m_fontSize;
}
VboParser * AssetManager::ProVboParser()
{
	return m_vboParser.get();
}
void AssetManager::AddEffect(string name, shared_ptr<DGSLEffect> effect)
{
	m_effects.insert(std::pair<string, shared_ptr<DGSLEffect>>(name, effect));
}