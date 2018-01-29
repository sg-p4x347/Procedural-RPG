#pragma once
#include "VboParser.h"
#include "AssetEntityManager.h"

class AssetManager
{
public:
	static AssetManager * Get();
	void SetAssetDir(Filesystem::path assets);
	void SetProceduralAssetDir(Filesystem::path procedural);
	void CleanupProceduralAssets();
	void SetDevice(Microsoft::WRL::ComPtr<ID3D11Device> device);
	//----------------------------------------------------------------
	// Asset Retrieval
	shared_ptr<SpriteFont> GetFont(string path);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture(string path,bool procedural = false);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetWicTexture(string path);
	std::shared_ptr<Model> GetModel(string path, float distance = 0.f, bool procedural = false);
	template <typename EffectType>
	inline bool GetEffect(string name, shared_ptr<EffectType> & effect) {
		effect = nullptr;
		auto result = m_effects.find(name);
		if (result != m_effects.end()) {
			effect = dynamic_pointer_cast<EffectType>(result->second);
		}
		return effect != nullptr;
	}
	int GetFontSize();
	VboParser * ProVboParser();
	//----------------------------------------------------------------
	// Asset Storage
	void AddEffect(string name, shared_ptr<DGSLEffect> effect);

	//----------------------------------------------------------------
	// Asset Creation
	void CreateEffects(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	
	AssetEntityManager * GetProceduralEM();
private:
	static AssetManager * m_instance;
	AssetManager();
	Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
	Filesystem::path m_authoredDir;
	Filesystem::path m_proceduralDir;
	Filesystem::path FullPath(string path, bool procedural,string extension);
	Filesystem::path AppendPath(string path, string type);

	//----------------------------------------------------------------
	// Entitities
	unique_ptr<AssetEntityManager> m_authoredEM;
	unique_ptr<AssetEntityManager> m_proceduralEM;
	//----------------------------------------------------------------
	// Textures
	map<string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_textures;
	map<string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_WicTextures;
	//----------------------------------------------------------------
	// Models
	
	unique_ptr<VboParser> m_vboParser;
	//----------------------------------------------------------------
	// Effects
	std::unique_ptr<DGSLEffectFactory>	m_DGSLfxFactory;
	std::unique_ptr<EffectFactory>		m_fxFactory;
	map<string, shared_ptr<IEffect>>	m_effects;
	
	//----------------------------------------------------------------
	// Shaders
	map<string,Microsoft::WRL::ComPtr<ID3D11PixelShader>>	m_pixelShaders;
	//----------------------------------------------------------------
	// Fonts
	map<string, std::shared_ptr<SpriteFont>> m_fonts;
	const int m_fontSize;

};

