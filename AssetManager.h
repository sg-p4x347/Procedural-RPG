#pragma once
#include "VboParser.h"
#include "AssetEntityManager.h"
#include "XmlParser.h"
#include "AssetTypes.h"
#include "HeightMap.h"
class AssetManager
{
public:
	static AssetManager * Get();
	void SetAssetDir(Filesystem::path assets);
	void SetProceduralAssetDir(Filesystem::path procedural);
	void CleanupProceduralAssets();
	void SetDevice(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void CollectGarbage();
	//----------------------------------------------------------------
	// Entity manager
	AssetEntityManager * GetStaticEM();
	AssetEntityManager * GetProceduralEM();
	//----------------------------------------------------------------
	// Asset Retrieval
	
	shared_ptr<XmlParser> GetXml(string path);
	shared_ptr<SpriteFont> GetFont(string name,int size);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture(string path,AssetType type = Authored);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetWicTexture(string path);
	std::shared_ptr<Model> GetModel(string path, float distance = 0.f,Vector3 position = Vector3::Zero, AssetType type = Authored);
	std::shared_ptr<Model> GetModel(unsigned int id, float distance = 0.f, Vector3 position = Vector3::Zero, AssetType type = Authored);
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
	// Heightmaps
	void CreateHeightMapModel(string path, HeightMap * heightMap, shared_ptr<Map<Vector3>> normalMap, float scaleFactor, int regionWidth, string effect);
	EntityPtr CreateHeightMap(string path, HeightMap * heightMap, float scaleFactor, int regionWidth);
	shared_ptr<Map<Vector3>> CreateNormalMap(HeightMap * heightMap);
	shared_ptr<Map<Vector3>> CreateNormalMap(int width, int length, std::function<float(int x, int y)> && getHeight);
	EntityPtr CreateNormalMap(string path, shared_ptr<Map<Vector3>> normalMap);
	shared_ptr<HeightMap> GetHeightMap(string path, AssetType type, Rectangle sampleArea, int sampleSpacing = 1);
	shared_ptr<Map<Vector3>> GetNormalMap(int mapWidth,string path, AssetType type, Rectangle sampleArea, int sampleSpacing = 1);
	int LOD(double distance, unsigned int regionWidth);
	std::shared_ptr<Model> CreateModelFromHeightMap(
		int regionWidth,
		HeightMap * heightMap,
		shared_ptr<Map<Vector3>> normalMap, 
		Rectangle sampleArea, 
		int sampleSpacing,
		shared_ptr<IEffect> effect);
	
	std::shared_ptr<Model> CreateTerrainModel(
		int mapWidth,
		int regionWidth,
		HeightMap * heightMap,
		shared_ptr<Map<Vector3>> normalMap,
		Rectangle sampleArea,
		int sampleSpacing,
		shared_ptr<IEffect> effect);
	float LowestNeighbor(HeightMap & water, HeightMap & terrain, int x, int z);
	
	// Query an asset entity by id
	bool Find(const unsigned int & id, AssetType & type, EntityPtr & entity);
	// Query an asset entity by path
	bool Find(AssetEntityManager * assetManager,string path, EntityPtr & entity);
	//----------------------------------------------------------------
	// Asset Storage
	void AddEffect(string name, shared_ptr<IEffect> effect);

	//----------------------------------------------------------------
	// Asset Creation
	void CreateDgslEffect(string name, vector<string> textures, const D3D11_INPUT_ELEMENT_DESC * inputElements, const UINT elementCount);
	void CreateCustomEffect(string name, vector<string> textures,const D3D11_INPUT_ELEMENT_DESC * inputElements,const UINT elementCount);
	void CreateEffects(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	void CreateInputLayouts();
	
	VertexPositionNormalTangentColorTexture CreateVertex(Vector3 position, Vector3 normal, Vector2 texture);
	
private:
	static AssetManager * m_instance;
	AssetManager();
	Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
	Filesystem::path m_authoredDir;
	Filesystem::path m_proceduralDir;
	Filesystem::path FullPath(string path, AssetType type,string extension);
	Filesystem::path FullPath(string path);
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
	std::shared_ptr<Model> GetModel(EntityPtr entity, float distance, Vector3 position, AssetType type);
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

