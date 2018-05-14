#include "pch.h"
#include "AssetManager.h"
#include "ModelAsset.h"
#include "CustomEffect.h"
#include "CustomModelLoadVBO.h"
#include "XmlParser.h"

AssetManager * AssetManager::m_instance = nullptr;
void AssetManager::CreateDgslEffect(string name, vector<string> textures, const D3D11_INPUT_ELEMENT_DESC * inputElements, const UINT elementCount)
{
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	m_pixelShaders.insert(std::pair<string, Microsoft::WRL::ComPtr<ID3D11PixelShader>>(name, pixelShader));
	auto blob = DX::ReadData(ansi2unicode("Assets\\" + name + ".cso").c_str()); // .cso is the compiled version of the hlsl shader (compiled shader object)
	DX::ThrowIfFailed(m_d3dDevice->CreatePixelShader(&blob.front(), blob.size(),
		nullptr, pixelShader.ReleaseAndGetAddressOf()));

	shared_ptr<DGSLEffect> effect = std::make_shared<DGSLEffect>(m_d3dDevice.Get(), pixelShader.Get());

	for (int textureIndex = 0; textureIndex < textures.size(); textureIndex++) {
		effect->SetTexture(textureIndex, AssetManager::Get()->GetTexture(textures[textureIndex]).Get());
	}


	effect->SetTextureEnabled(true);
	effect->SetVertexColorEnabled(true);
	effect->EnableDefaultLighting();

	// Input layout
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	void const* shaderByteCode;
	size_t byteCodeLength;

	effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	DX::ThrowIfFailed(m_d3dDevice->CreateInputLayout(
		inputElements,
		elementCount,
		shaderByteCode, byteCodeLength,
		inputLayout.ReleaseAndGetAddressOf()));
	m_inputLayouts.insert(std::make_pair(name, inputLayout));


	AddEffect(name, effect);
}
void AssetManager::CreateCustomEffect(string name, vector<string> textures,const D3D11_INPUT_ELEMENT_DESC * inputElements, const UINT elementCount)
{
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	m_vertexShaders.insert(std::pair<string, Microsoft::WRL::ComPtr<ID3D11VertexShader>>(name, vertexShader));
	auto blob = DX::ReadData(ansi2unicode("Assets\\VS" + name + ".cso").c_str()); // .cso is the compiled version of the hlsl shader (compiled shader object)
	DX::ThrowIfFailed(m_d3dDevice->CreateVertexShader(&blob.front(), blob.size(),
		nullptr, vertexShader.ReleaseAndGetAddressOf()));

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	m_pixelShaders.insert(std::pair<string, Microsoft::WRL::ComPtr<ID3D11PixelShader>>(name, pixelShader));
	blob = DX::ReadData(ansi2unicode("Assets\\" + name + ".cso").c_str()); // .cso is the compiled version of the hlsl shader (compiled shader object)
	DX::ThrowIfFailed(m_d3dDevice->CreatePixelShader(&blob.front(), blob.size(),
		nullptr, pixelShader.ReleaseAndGetAddressOf()));

	shared_ptr<CustomEffect> effect = std::make_shared<CustomEffect>(m_d3dDevice.Get(), pixelShader.Get(),vertexShader.Get());

	for (int textureIndex = 0; textureIndex < textures.size(); textureIndex++) {
		effect->SetTexture(textureIndex,AssetManager::Get()->GetTexture(textures[textureIndex]).Get());
	}

	/*effect->SetTextureEnabled(true);
	effect->SetVertexColorEnabled(true);

	effect->EnableDefaultLighting();*/
	// Input layout
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	void const* shaderByteCode;
	size_t byteCodeLength;

	effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	DX::ThrowIfFailed(m_d3dDevice->CreateInputLayout(
		inputElements,
		elementCount,
		shaderByteCode, byteCodeLength,
		inputLayout.ReleaseAndGetAddressOf()));
	m_inputLayouts.insert(std::make_pair(name, inputLayout));

	AddEffect(name, effect);
}
void AssetManager::CreateEffects(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	//----------------------------------------------------------------
	// Terrain Effect
	CreateDgslEffect(
		"Terrain",
		vector<string>{"dirt", "grass", "stone", "snow"},
		VertexPositionNormalTangentColorTexture::InputElements,
		VertexPositionNormalTangentColorTexture::InputElementCount
	);
	//----------------------------------------------------------------
	// Water Effect
	CreateDgslEffect(
		"Water",
		vector<string>{"water"},
		VertexPositionNormalTangentColorTexture::InputElements,
		VertexPositionNormalTangentColorTexture::InputElementCount
	);
	//----------------------------------------------------------------
	// Wood Effect
	CreateDgslEffect(
		"Wood",
		vector<string>{"wood"},
		VertexPositionNormalTangentColorTexture::InputElements,
		VertexPositionNormalTangentColorTexture::InputElementCount
	);
	//----------------------------------------------------------------
	// Test Effect
	CreateDgslEffect(
		"Test",
		vector<string>{"test"},
		VertexPositionNormalTangentColorTexture::InputElements,
		VertexPositionNormalTangentColorTexture::InputElementCount
	);

	//Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	//m_pixelShaders.insert(std::pair<string, Microsoft::WRL::ComPtr<ID3D11PixelShader>>("Terrain", pixelShader));
	//auto blob = DX::ReadData(L"Assets\\Terrain.cso"); // .cso is the compiled version of the hlsl shader (compiled shader object)
	//DX::ThrowIfFailed(m_d3dDevice->CreatePixelShader(&blob.front(), blob.size(),
	//	nullptr, pixelShader.ReleaseAndGetAddressOf()));

	//shared_ptr<DGSLEffect> terrain = std::make_shared<DGSLEffect>(m_d3dDevice.Get(), pixelShader.Get());

	//terrain->SetTexture(AssetManager::Get()->GetTexture("dirt").Get());
	//terrain->SetTexture(1, AssetManager::Get()->GetTexture("grass").Get());
	//terrain->SetTexture(2, AssetManager::Get()->GetTexture("stone").Get());
	//terrain->SetTexture(3, AssetManager::Get()->GetTexture("snow").Get());

	//terrain->SetTextureEnabled(true);
	//terrain->SetVertexColorEnabled(true);

	//terrain->EnableDefaultLighting();
	//// Input layout
	//{
	//	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	//	void const* shaderByteCode;
	//	size_t byteCodeLength;

	//	terrain->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	//	DX::ThrowIfFailed(m_d3dDevice->CreateInputLayout(
	//		VertexPositionNormalTangentColorTexture::InputElements,
	//		VertexPositionNormalTangentColorTexture::InputElementCount,
	//		shaderByteCode, byteCodeLength,
	//		inputLayout.ReleaseAndGetAddressOf()));
	//	m_inputLayouts.insert(std::make_pair())
	//}

	
	/*DGSLEffectFactory::DGSLEffectInfo info;
	info.name = L"Water";
	info.pixelShader = L"Assets\\Water.cso";
	shared_ptr<DGSLEffect> water = std::dynamic_pointer_cast <DGSLEffect>(m_DGSLfxFactory->CreateDGSLEffect(info, context.Get()));
	water->SetTexture(AssetManager::Get()->GetTexture("water").Get());
	water->SetTextureEnabled(true);
	water->SetVertexColorEnabled(true);
	water->EnableDefaultLighting();
	
	void const* shaderByteCode;
	size_t byteCodeLength;

	water->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	DX::ThrowIfFailed(m_d3dDevice->CreateInputLayout(
		VertexPositionNormalTangentColorTexture::InputElements,
		VertexPositionNormalTangentColorTexture::InputElementCount,
		shaderByteCode, byteCodeLength,
		m_waterLayout.ReleaseAndGetAddressOf()));

	
	AddEffect("Water", water);*/
}
void AssetManager::CreateInputLayouts()
{
	/*shared_ptr<DGSLEffect> terrain;
	if (AssetManager::Get()->GetEffect<DGSLEffect>("Terrain", terrain))
	
	shared_ptr<DGSLEffect> water;
	if (AssetManager::Get()->GetEffect<DGSLEffect>("Water", water))
	{
		
	}*/
}
void AssetManager::CreateHeightMapModel(string path, shared_ptr<HeightMap> heightMap,shared_ptr<Map<Vector3>> normalMap, float scaleFactor, int regionWidth, string effect)
{
	EntityPtr hmapEntity = CreateHeightMap(path, heightMap, scaleFactor, regionWidth);
	hmapEntity->AddComponent(new ModelAsset(0, 0, effect));
	CreateNormalMap(path + "_normal",CreateNormalMap(heightMap));
}
EntityPtr AssetManager::CreateHeightMap(string path, shared_ptr<HeightMap> heightMap, float scaleFactor, int regionWidth)
{
	EntityPtr entity = m_proceduralEM->NewEntity();
	entity->AddComponent(new PathID(path));
	entity->AddComponent(new HeightMapAsset(heightMap->width, heightMap->length, scaleFactor,regionWidth));
	// Compress the vertices into shorts, packed into a char array
	unsigned int vertexCount = (heightMap->width + 1) * (heightMap->length + 1);
	unique_ptr<unsigned char[]> buffer(new unsigned char[vertexCount * sizeof(short)]);
	unsigned int index = 0;
	unsigned int biomeIndex = 0;

	for (unsigned short vertY = 0; vertY <= heightMap->length; vertY++) {
		for (unsigned short vertX = 0; vertX <= heightMap->width; vertX++) {
			float vertex = heightMap->map[vertX][vertY];
			short vertexShort = (short)(vertex * scaleFactor);
			std::memcpy(&buffer.get()[index * sizeof(short)], &vertexShort, sizeof(short));
		}
	}
	// save the heightmap to the file
	ofstream ofs(FullPath(path, Procedural, ".dat"), std::ios::binary);
	ofs.write((const char *)buffer.get(), vertexCount * sizeof(short));

	return entity;
}
shared_ptr<Map<Vector3>> AssetManager::CreateNormalMap(shared_ptr<HeightMap> heightMap)
{
	shared_ptr<Map<Vector3>> normalMap = std::shared_ptr<Map<Vector3>>(new Map<Vector3>(heightMap->width, heightMap->length));
	for (int x = 0; x <= heightMap->width;x++) {
		for (int y = 0; y <= heightMap->length;y++) {
			float vertex = heightMap->map[x][y];
			// adjacent vertices, if on the edge, this vertex is used
			float left = heightMap->Bounded(x-1,y) ? heightMap->map[x - 1][y] : vertex;
			float right = heightMap->Bounded(x + 1, y) ? heightMap->map[x + 1][y] : vertex;
			float up = heightMap->Bounded(x, y +1) ? heightMap->map[x][y + 1] : vertex;
			float down = heightMap->Bounded(x , y-1) ? heightMap->map[x][y - 1] : vertex;

			Vector3 normal = DirectX::SimpleMath::Vector3(left - right, 2.f, down - up);
			normal.Normalize();
			normalMap->map[x][y] = normal;
		}
	}
	return normalMap;
}
EntityPtr AssetManager::CreateNormalMap(string path, shared_ptr<Map<Vector3>> normalMap)
{
	EntityPtr entity = m_proceduralEM->NewEntity();
	entity->AddComponent(new PathID(path));
	entity->AddComponent(new NormalMapAsset(normalMap->width, normalMap->length));
	// Compress the vertices into shorts, packed into a char array
	unsigned int vertexCount = (normalMap->width + 1) * (normalMap->length + 1);
	unique_ptr<unsigned char[]> buffer(new unsigned char[vertexCount * sizeof(short)]);
	unsigned int index = 0;

	for (unsigned short vertY = 0; vertY <= normalMap->length; vertY++) {
		for (unsigned short vertX = 0; vertX <= normalMap->width; vertX++) {
			Vector3 normal = normalMap->map[vertX][vertY];
			// scale the vector up into the 8-bit range
			normal *= 255;
			buffer.get()[index * 3] = int8_t(normal.x);
			buffer.get()[index * 3 + 1] = int8_t(normal.y);
			buffer.get()[index * 3 + 2] = int8_t(normal.z);
			// update vertex index
			index++;
		}
	}
	// save the normalMap to the file
	ofstream ofs(FullPath(path, Procedural, ".dat"), std::ios::binary);
	ofs.write((const char *)buffer.get(), vertexCount * sizeof(short));

	return entity;
}
AssetEntityManager * AssetManager::GetProceduralEM()
{
	return m_proceduralEM.get();
}
AssetManager::AssetManager() : m_fontSize(32)
{

}

Filesystem::path AssetManager::FullPath(string path, AssetType type, string extension)
{
	Filesystem::path rootDir;
	switch (type) {
	case Authored: rootDir = m_authoredDir; break;
	case Procedural: rootDir = m_proceduralDir;break;
	}

	return Filesystem::path(rootDir / (path + extension));
}

Filesystem::path AssetManager::FullPath(string path)
{
	return Filesystem::path(m_authoredDir / path);
}

Filesystem::path AssetManager::AppendPath(string path, string type)
{
	return Filesystem::path( m_authoredDir / type / path);
}

std::shared_ptr<Model> AssetManager::GetModel(EntityPtr entity, float distance, Vector3 position, AssetType type)
{
	std::shared_ptr<Model> model;
	try {
		string path = entity->GetComponent<PathID>("PathID")->Path;
		try {
			if (m_d3dDevice == nullptr) throw std::exception("AssetManager device not set");
			// get the components
			shared_ptr<ModelAsset> modelAsset = entity->GetComponent<ModelAsset>("ModelAsset");
			shared_ptr<HeightMapAsset> heightMapComp = entity->GetComponent<HeightMapAsset>("HeightMapAsset");
			if (!heightMapComp) {
				//----------------------------------------------------------------
				// Standard models

				// get the LOD level
				int lod = std::min(modelAsset->LodCount - 1, (int)distance / modelAsset->LodSpacing);
				// Get the model
				
				if (modelAsset->LODs.size() <= lod || !modelAsset->LODs[lod]) {
					//----------------------------------------------------------------
					// Cache the LOD

					// get the path
					Filesystem::path fullPath = FullPath(path + '_' + std::to_string(lod), type, type == Procedural ? ".vbo" : ".cmo");
					// Load from file
					if (type == Procedural) {
						//model.reset(Model::CreateFromVBO(m_d3dDevice.Get(), fullPath.c_str()).release());
						shared_ptr<IEffect> effect;
						GetEffect(modelAsset->Effect, effect);
						model.reset(CustomModelLoadVBO::CreateFromVBO(m_d3dDevice.Get(), fullPath.string(), effect).release());
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
				
			}
			else {
				//----------------------------------------------------------------
				// Heightmap models

				if (modelAsset->RegionModels.width == 0) modelAsset->RegionModels.Resize(
					heightMapComp->Xsize / heightMapComp->RegionWidth,
					heightMapComp->Ysize / heightMapComp->RegionWidth);

				Rectangle sampleArea;
				// Get the region coordinates
				int x = (int)std::floor(position.x / (double)heightMapComp->RegionWidth);
				int z = (int)std::floor(position.z / (double)heightMapComp->RegionWidth);

				int lod = LOD(distance, heightMapComp->RegionWidth);
				if (modelAsset->RegionModels.map[x][z].second && modelAsset->RegionModels.map[x][z].first == lod) {
					unsigned int sampleSpacing = std::pow(2, lod);
					
					auto heightMap = GetHeightMap(path, type, sampleArea, sampleSpacing);
					auto normalMap = GetNormalMap(path + "_normal", type, sampleArea, sampleSpacing);

					shared_ptr<IEffect> effect;
					GetEffect(modelAsset->Effect, effect);
					model = CreateModelFromHeightMap(heightMap, normalMap, sampleArea, sampleSpacing, effect);
					// Cach the model;
					modelAsset->RegionModels.map[x][z].second = model;
				}
				else {
					model = modelAsset->RegionModels.map[x][z].second;
				}
			}
		}
		catch (std::exception ex) {
			Utility::OutputException(path + ' ' + ex.what());
		}
	}
	catch (std::exception ex) {
		Utility::OutputException("Could not get PathID from model asset");
	}
	return model;
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

AssetEntityManager * AssetManager::GetStaticEM()
{
	return m_authoredEM.get();
}

shared_ptr<HeightMap> AssetManager::GetHeightMap(
	string path, 
	AssetType type, 
	// The precise area for which to sample
	Rectangle sampleArea,
	// Each sampled x and y coordinate is scaled by this
	int sampleSpacing
)
{
	// Get the heightmap asset entity
	EntityPtr entity;
	if (Find(path, entity)) {
		// Get the heightmap component
		auto heightMapComp = entity->GetComponent<HeightMapAsset>("HeightMapAsset");
		if (heightMapComp) {
			// Load the vertex array with data.
			int xSize = sampleArea.width / sampleSpacing;
			int ySize = sampleArea.height / sampleSpacing;
			shared_ptr<HeightMap> heightMap = std::make_shared<HeightMap>(xSize,ySize);
			ifstream stream(FullPath(path,type,".dat"), ios::binary);


			if (stream.is_open()) {
				// stores the exact bytes from the file into memory
				// move start position to the region, and proceed to read each line into the Char buffers


				for (int vertY = 0; vertY <= ySize; vertY++) {
					for (int vertX = 0; vertX <= xSize; vertX++) {
						int globalX = vertX * sampleSpacing + sampleArea.x;
						int globalY = vertY * sampleSpacing + sampleArea.y;
						int index = Utility::posToIndex(globalX, globalY, heightMapComp->Xsize + 1);

						char shortBuffer[2];
						stream.seekg(index * heightMapComp->RegionWidth);
						stream.read((char *)&shortBuffer, heightMapComp->RegionWidth);

						short vertex = 0;
						memcpy(&vertex, &shortBuffer, heightMapComp->RegionWidth);
						 
						heightMap->map[vertX][vertY] = (float)vertex / heightMapComp->ScaleFactor;
					}
				}

				stream.close();
			}
			return heightMap;
		}
	}
	return nullptr;
}

shared_ptr<Map<Vector3>> AssetManager::GetNormalMap(string path, AssetType type, Rectangle sampleArea, int sampleSpacing)
{
	// Get the heightmap asset entity
	EntityPtr entity;
	if (Find(path, entity)) {
		// Get the heightmap component
		auto heightMapComp = entity->GetComponent<HeightMapAsset>("HeightMapAsset");
		if (heightMapComp) {
			// Load the vertex array with data.
			int xSize = sampleArea.width;
			int ySize = sampleArea.height;
			shared_ptr<Map<Vector3>> normalMap = std::make_shared<Map<Vector3>>(xSize, ySize);
			ifstream stream(FullPath(path, type, ".dat"), ios::binary);


			if (stream.is_open()) {
				// stores the exact bytes from the file into memory
				// move start position to the region, and proceed to read each line into the Char buffers


				for (int vertY = 0; vertY <= ySize; vertY++) {
					for (int vertX = 0; vertX <= xSize; vertX++) {
						int globalX = vertX * sampleSpacing + sampleArea.x;
						int globalY = vertY * sampleSpacing + sampleArea.y;
						int index = Utility::posToIndex(globalX, globalY, heightMapComp->Xsize + 1);

						char normalBuffer[3];
						stream.seekg(index * 3 * sizeof(char));
						stream.read((char *)normalBuffer, 3 * sizeof(char));
						Vector3 normal = Vector3(float(normalBuffer[0]), float(normalBuffer[1]), float(normalBuffer[2]));
						normal.Normalize();
						normalMap->map[vertX][vertY] = normal;
					}
				}

				stream.close();
				return normalMap;
			}
		}
	}
	return nullptr;
}

int AssetManager::LOD(double distance, unsigned int regionWidth)
{
	return 0 + std::min((int)std::log2(regionWidth), std::max(0, (int)std::floor(std::log2(distance / (double)regionWidth))));
}

shared_ptr<XmlParser> AssetManager::GetXml(string path)
{
	ifstream ifs = ifstream(FullPath(path));
	return make_shared<XmlParser>(ifs);
}

shared_ptr<SpriteFont> AssetManager::GetFont(string name,int size)
{
	string path = name + '_' + to_string(size);
	try {

		// Search cache
		if (m_fonts.find(path) != m_fonts.end()) {
			return m_fonts[path];
		}
		if (m_d3dDevice == nullptr) throw std::exception("AssetManager device not set");
		// get the path
		Filesystem::path fullPath = FullPath(path, Authored, ".spritefont");
		// Load from file
		shared_ptr<SpriteFont> font = std::make_shared<SpriteFont>(m_d3dDevice.Get(),fullPath.c_str());
		m_fonts.insert(std::pair<string, shared_ptr<SpriteFont>>(path, font));
		return font;
	}
	catch (std::exception ex) {
		Utility::OutputException(path + ' ' + ex.what());
	}
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> AssetManager::GetTexture(string path, AssetType type)
{
	try {
		
		// Search cache
		if (m_textures.find(path) != m_textures.end()) {
			return m_textures[path];
		}
		if (m_d3dDevice == nullptr) throw std::exception("AssetManager device not set");
		// get the path
		Filesystem::path fullPath = FullPath(path, type, ".dds");
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

std::shared_ptr<Model> AssetManager::GetModel(string path, float distance, Vector3 position, AssetType type)
{
	EntityPtr entity;
	switch(type) {
	case Procedural: m_proceduralEM->TryFindByPathID(path, entity);break;
	case Authored: m_authoredEM->TryFindByPathID(path, entity); break;
	}
	return GetModel(entity, distance,position, type);
}
std::shared_ptr<Model> AssetManager::GetModel(unsigned int id, float distance, Vector3 position, AssetType type)
{
	EntityPtr entity;
	switch (type) {
	case Procedural: m_proceduralEM->BaseEntityManager::Find(id, entity);break;
	case Authored: m_authoredEM->BaseEntityManager::Find(id, entity); break;
	}
	return GetModel(entity, distance,position, type);
}

std::shared_ptr<Model> AssetManager::CreateModelFromHeightMap(
	shared_ptr<HeightMap> heightMap,
	shared_ptr<Map<Vector3>> normalMap, 
	Rectangle sampleArea, 
	int sampleSpacing,
	shared_ptr<IEffect> effect)
{
	// create 2 triangles (6 vertices) for every quad in the region
	vector<VertexPositionNormalTangentColorTexture> vertices(6 * heightMap->width * heightMap->length);

	vector<uint16_t> indices(6 * heightMap->width * heightMap->length);
	int index = 0;
	for (int z = 0; z < heightMap->length; z++) {
		for (int x = 0; x < heightMap->width; x++) {
			// Get the indexes to the four points of the quad.

			// Upper left.
			Vector3 vertex1(
				(float)(x * sampleSpacing + sampleArea.x),
				heightMap->map[x][z],
				(float)(z * sampleSpacing + sampleArea.y));
			// Upper right.
			Vector3 vertex2(
				(float)((x + 1) * sampleSpacing + sampleArea.x),
				heightMap->map[x + 1][z],
				(float)(z * sampleSpacing + sampleArea.y));
			// Bottom left.
			Vector3 vertex3(
				(float)(x * sampleSpacing + sampleArea.x),
				heightMap->map[x][z + 1],
				(float)((z + 1) * sampleSpacing + sampleArea.y));
			// Bottom right.
			Vector3 vertex4(
				(float)((x + 1) * sampleSpacing + sampleArea.x),
				heightMap->map[x + 1][z + 1],
				(float)((z + 1) * sampleSpacing + sampleArea.y));

			/*
			1---2
			| \ |
			3---4
			*/

			// Triangle 1 - Upper left
			vertices[index] = CreateVertex(
				XMFLOAT3(vertex1),				// position
				XMFLOAT3(normalMap->map[x][z]),	// normal
				XMFLOAT2(0.f, 0.f)				// texture
			);
			indices[index] = index;
			index++;
			// Triangle 1 - Bottom right.
			vertices[index] = CreateVertex(
				XMFLOAT3(vertex4),										// position
				XMFLOAT3(normalMap->map[x + 1][z + 1]),	// normal
				XMFLOAT2(1.f, 1.f)										// texture
			);
			indices[index] = index;
			index++;
			// Triangle 1 - Bottom left.
			vertices[index] = CreateVertex(
				XMFLOAT3(vertex3),											// position
				XMFLOAT3(normalMap->map[x][z + 1]),	// normal
				XMFLOAT2(0.f, 1.f)											// texture
			);
			indices[index] = index;
			index++;
			// Triangle 2 - Upper left.
			vertices[index] = CreateVertex(
				XMFLOAT3(vertex1),										// position
				XMFLOAT3(normalMap->map[x][z]),	// normal
				XMFLOAT2(0.f, 0.f)										// texture
			);
			indices[index] = index;
			index++;
			// Triangle 2 - Upper right.
			vertices[index] = CreateVertex(
				XMFLOAT3(vertex2),											// position
				XMFLOAT3(normalMap->map[x + 1][z]),	// normal
				XMFLOAT2(1.f, 0.f)											// texture
			);
			indices[index] = index;
			index++;
			// Triangle 2 - Bottom right.
			vertices[index] = CreateVertex(
				XMFLOAT3(vertex4),												// position
				XMFLOAT3(normalMap->map[x + 1][z + 1]),	// normal
				XMFLOAT2(1.f, 1.f)												// texture
			);
			indices[index] = index;
			index++;
		}
	}
	return CustomModelLoadVBO::CreateFromVBO(m_d3dDevice.Get(), vertices, indices, effect);
}
VertexPositionNormalTangentColorTexture AssetManager::CreateVertex(Vector3 position, Vector3 normal, Vector2 texture)
{
	return VertexPositionNormalTangentColorTexture(position, normal, Vector4::Zero, Vector4::Zero, texture);
}
Microsoft::WRL::ComPtr<ID3D11InputLayout> AssetManager::GetInputLayout(string name)
{
	if (m_inputLayouts.find(name) != m_inputLayouts.end()) {
		return m_inputLayouts[name];
	}
	return nullptr;
}
int AssetManager::GetFontSize()
{
	return m_fontSize;
}
VboParser * AssetManager::ProVboParser()
{
	return m_vboParser.get();
}
bool AssetManager::Find(string path, EntityPtr & entity)
{
	return m_authoredEM->TryFindByPathID(path, entity);
}
void AssetManager::AddEffect(string name, shared_ptr<IEffect> effect)
{
	m_effects.insert(std::pair<string, shared_ptr<IEffect>>(name, effect));
}
