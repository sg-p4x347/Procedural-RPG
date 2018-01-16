#pragma once
class AssetManager
{
public:
	static AssetManager * Get();
	void SetAssetDir(Filesystem::path assets);
	void SetProceduralAssetDir(Filesystem::path procedural);
	void SetDevice(Microsoft::WRL::ComPtr<ID3D11Device> device);
	//----------------------------------------------------------------
	// Asset Retrieval
	shared_ptr<SpriteFont> GetFont(string path);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture(string path,bool procedural = false);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetWicTexture(string path);
	std::shared_ptr<Model> GetModel(string path, bool procedural = false);
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
	//----------------------------------------------------------------
	// Asset Storage
	void AddEffect(string name, shared_ptr<DGSLEffect> effect);

	//----------------------------------------------------------------
	// Asset Creation
	void CreateEffects(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
private:
	static AssetManager * m_instance;
	AssetManager();
	Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
	Filesystem::path m_authoredDir;
	Filesystem::path m_proceduralDir;
	Filesystem::path FullPath(string path, bool procedural,string type,string extension);
	Filesystem::path AppendPath(string path, string type);
	//----------------------------------------------------------------
	// Textures
	map<string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_textures;
	map<string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_WicTextures;
	//----------------------------------------------------------------
	// Models
	map<string, std::shared_ptr<Model>> m_models;

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

