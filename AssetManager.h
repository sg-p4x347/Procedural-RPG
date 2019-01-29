#pragma once
#include "VboParser.h"
#include "AssetEntityManager.h"
#include "XmlParser.h"
#include "AssetTypes.h"
#include "AssetTypedefs.h"
#include "HeightMap.h"
#include "CMF.h"
class AssetManager
{
public:
	static AssetManager * Get();
	void SetAssetDir(Filesystem::path assets);
	void SetProceduralAssetDir(Filesystem::path procedural);
	void CleanupProceduralAssets();
	IEffectFactory * GetFxFactory();
	ID3D11Device * GetDevice();
	void SetDevice(Microsoft::WRL::ComPtr<ID3D11Device> device);

	void CollectGarbage();
	//----------------------------------------------------------------
	// Entity manager
	AssetEntityManager * GetStaticEM();
	AssetEntityManager * GetProceduralEM();
	//----------------------------------------------------------------
	// Asset compilation
	void CompileFbxAssets();
	void CompileFbxAsset(Filesystem::path path);
	//----------------------------------------------------------------
	// Asset Retrieval
	
	shared_ptr<XmlParser> GetXml(string path);
	shared_ptr<SpriteFont> GetFont(string name,int size);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture(string path,AssetType type = Authored);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetWicTexture(string path);
	std::shared_ptr<geometry::CMF> GetModel(string path, AssetType type = Authored);
	std::shared_ptr<geometry::CMF> GetModel(AssetID id, AssetType type = Authored);
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
	template<typename T>
	shared_ptr<Map<T>> GetMap(string name, DirectX::SimpleMath::Rectangle sampleArea, int sampleSpacing = 1);
	//----------------------------------------------------------------
	// Heightmaps
	void CreateHeightMapModel(string path, HeightMap * heightMap, shared_ptr<Map<Vector3>> normalMap, float scaleFactor, int regionWidth, string effect);
	EntityPtr CreateHeightMap(string path, HeightMap * heightMap, float scaleFactor, int regionWidth);
	shared_ptr<Map<Vector3>> CreateNormalMap(HeightMap * heightMap);
	shared_ptr<Map<Vector3>> CreateNormalMap(int width, int length, std::function<float(int x, int y)> && getHeight);
	EntityPtr CreateNormalMap(string path, shared_ptr<Map<Vector3>> normalMap);
	shared_ptr<HeightMap> GetHeightMap(string path, AssetType type, DirectX::SimpleMath::Rectangle sampleArea, int sampleSpacing = 1);
	void UpdateHeightMap(string path, AssetType type, shared_ptr<HeightMap> map, DirectX::SimpleMath::Rectangle domain);
	shared_ptr<Map<Vector3>> GetNormalMap(int mapWidth,string path, AssetType type, DirectX::SimpleMath::Rectangle sampleArea, int sampleSpacing = 1);
	int LOD(double distance, unsigned int regionWidth);
	std::shared_ptr<Model> CreateModelFromHeightMap(
		int regionWidth,
		HeightMap * heightMap,
		shared_ptr<Map<Vector3>> normalMap, 
		DirectX::SimpleMath::Rectangle sampleArea,
		int sampleSpacing,
		shared_ptr<IEffect> effect);
	
	std::shared_ptr<Model> CreateTerrainModel(
		int mapWidth,
		int regionWidth,
		HeightMap * heightMap,
		shared_ptr<Map<Vector3>> normalMap,
		DirectX::SimpleMath::Rectangle sampleArea,
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
	AssetID AddModel(std::shared_ptr<geometry::CMF> model, AssetType type = AssetType::Procedural);
	void CreateDgslEffect(string name, vector<string> textures, const D3D11_INPUT_ELEMENT_DESC * inputElements, const UINT elementCount);
	void CreateCustomEffect(string name, vector<string> textures,const D3D11_INPUT_ELEMENT_DESC * inputElements,const UINT elementCount);
	void CreateEffects(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	void CreateInputLayouts();
	template<typename T>
	void CreateMap(string name, Map<T> & map);
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
	map<std::pair<AssetID,AssetType>,std::shared_ptr<geometry::CMF>> m_cmfCache;
	std::shared_ptr<geometry::CMF> GetModel(EntityPtr entity, AssetType type);
	
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

template<typename T>
inline shared_ptr<Map<T>> AssetManager::GetMap(string name, DirectX::SimpleMath::Rectangle sampleArea, int sampleSpacing)
{
	EntityPtr entity;
	if (m_proceduralEM->TryFindByPathID(name, entity)) {
		// Get the heightmap component
		auto heightMapComp = entity->GetComponent<HeightMapAsset>("HeightMapAsset");
		if (heightMapComp) {
			// Load the vertex array with data.
			int xSize = sampleArea.width / sampleSpacing;
			int ySize = sampleArea.height / sampleSpacing;
			shared_ptr<Map<T>> map = std::make_shared<Map<T>>(xSize, ySize);
			map->area.x = sampleArea.x;
			map->area.y = sampleArea.y;
			ifstream stream(FullPath(path, AssetType::Authored, ".map"), std::ios_base::binary);


			if (stream.is_open()) {
				// stores the exact bytes from the file into memory
				// move start position to the region, and proceed to read each line into the Char buffers


				for (int vertY = 0; vertY <= ySize; vertY++) {
					for (int vertX = 0; vertX <= xSize; vertX++) {
						int globalX = vertX * sampleSpacing + sampleArea.x;
						int globalY = vertY * sampleSpacing + sampleArea.y;
						int index = Utility::posToIndex(globalX, globalY, heightMapComp->Xsize + 1);

						stream.seekg(index * sizeof(T));
						stream.read((char *)&(*map)[vertX][vertY], sizeof(T));
					}
				}

				stream.close();
			}
			return map;
		}
	}
	return nullptr;
}

template<typename T>
inline void AssetManager::CreateMap(string name, Map<T>& map)
{
	auto entity = m_proceduralEM->CreateAsset(name);
	entity->AddComponent(new HeightMapAsset(map.width, map.length, 1.f));
	// save the map to the file
	ofstream ofs(FullPath(name, Procedural, ".map"), std::ios::binary);
	for (unsigned short vertY = 0; vertY <= map.length; vertY++) {
		for (unsigned short vertX = 0; vertX <= map.width; vertX++) {
			ofs.write((const char *)&map[vertX][vertY], sizeof(T));
		}
	}
	ofs.close();
}
