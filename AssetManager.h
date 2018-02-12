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
	Microsoft::WRL::ComPtr<ID3D11InputLayout> GetInputLayout(string name);
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
	void AddEffect(string name, shared_ptr<IEffect> effect);

	//----------------------------------------------------------------
	// Asset Creation
	void CreateDgslEffect(string name, vector<string> textures, const D3D11_INPUT_ELEMENT_DESC * inputElements, const UINT elementCount);
	void CreateCustomEffect(string name, vector<string> textures,const D3D11_INPUT_ELEMENT_DESC * inputElements,const UINT elementCount);
	void CreateEffects(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	void CreateInputLayouts();
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
	// Input Layouts
	map<string,Microsoft::WRL::ComPtr<ID3D11InputLayout>>	m_inputLayouts;
	//----------------------------------------------------------------
	// Shaders
	map<string,Microsoft::WRL::ComPtr<ID3D11PixelShader>>	m_pixelShaders;
	map<string, Microsoft::WRL::ComPtr<ID3D11VertexShader>>	m_vertexShaders;
	//----------------------------------------------------------------
	// Fonts
	map<string, std::shared_ptr<SpriteFont>> m_fonts;
	const int m_fontSize;

};

