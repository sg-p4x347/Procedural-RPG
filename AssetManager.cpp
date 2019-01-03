#include "pch.h"
#include "AssetManager.h"
#include "ModelAsset.h"
#include "CustomEffect.h"
#include "CustomModelLoadVBO.h"
#include "CustomModelLoadCMO.h"
#include "XmlParser.h"
#include "Filesystem.h"
#include "CMF.h"
using CMF = geometry::CMF;
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
void AssetManager::CreateHeightMapModel(string path, HeightMap * heightMap,shared_ptr<Map<Vector3>> normalMap, float scaleFactor, int regionWidth, string effect)
{
	EntityPtr hmapEntity = CreateHeightMap(path, heightMap, scaleFactor, regionWidth);
	hmapEntity->AddComponent(new ModelAsset(0, 0, effect));
	CreateNormalMap(path + "_normal",normalMap);
}
EntityPtr AssetManager::CreateHeightMap(string path, HeightMap * heightMap, float scaleFactor, int regionWidth)
{
	EntityPtr entity = m_proceduralEM->NewEntity();
	entity->AddComponent(new PathID(path));
	entity->AddComponent(new HeightMapAsset(heightMap->width, heightMap->length, scaleFactor,regionWidth));
	// Compress the vertices into shorts, packed into a char array
	unsigned int vertexCount = (heightMap->width + 1) * (heightMap->length + 1);
	unique_ptr<unsigned char[]> buffer(new unsigned char[vertexCount * sizeof(short)]);
	unsigned int index = 0;

	for (unsigned short vertY = 0; vertY <= heightMap->length; vertY++) {
		for (unsigned short vertX = 0; vertX <= heightMap->width; vertX++) {
			float vertex = heightMap->map[vertX][vertY];
			short vertexShort = (short)(vertex * scaleFactor);
			std::memcpy(&buffer.get()[index * sizeof(short)], &vertexShort, sizeof(short));
			index++;
		}
	}
	// save the heightmap to the file
	ofstream ofs(FullPath(path, Procedural, ".dat"), std::ios::binary);
	ofs.write((const char *)buffer.get(), vertexCount * sizeof(short));

	return entity;
}
shared_ptr<Map<Vector3>> AssetManager::CreateNormalMap(HeightMap * heightMap)
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
shared_ptr<Map<Vector3>> AssetManager::CreateNormalMap(int width, int length, std::function<float(int x, int y)> && getHeight)
{
	shared_ptr<Map<Vector3>> normalMap = std::shared_ptr<Map<Vector3>>(new Map<Vector3>(width, length));
	for (int x = 0; x <= width;x++) {
		for (int y = 0; y <= length;y++) {
			float vertex = getHeight(x, y);
			// adjacent vertices, if on the edge, this vertex is used
			float left = x - 1 >= 0 ? getHeight(x - 1,y) : vertex;
			float right = x + 1 <= width ? getHeight(x + 1,y) : vertex;
			float up = y + 1 <= length ? getHeight(x,y + 1) : vertex;
			float down = y - 1 >= 0 ? getHeight(x,y - 1) : vertex;

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
	unique_ptr<unsigned char[]> buffer(new unsigned char[vertexCount * sizeof(int8_t) * 3]);
	unsigned int index = 0;

	for (unsigned short vertY = 0; vertY <= normalMap->length; vertY++) {
		for (unsigned short vertX = 0; vertX <= normalMap->width; vertX++) {
			Vector3 normal = normalMap->map[vertX][vertY];
			// scale the vector up into the 8-bit range
			normal *= 127;
			buffer.get()[index * 3] = int8_t(normal.x);
			buffer.get()[index * 3 + 1] = int8_t(normal.y);
			buffer.get()[index * 3 + 2] = int8_t(normal.z);
			// update vertex index
			index++;
		}
	}
	// save the normalMap to the file
	ofstream ofs(FullPath(path, Procedural, ".dat"), std::ios::binary);
	ofs.write((const char *)buffer.get(), vertexCount * sizeof(int8_t) * 3);

	return entity;
}
AssetEntityManager * AssetManager::GetProceduralEM()
{
	return m_proceduralEM.get();
}
void AssetManager::CompileFbxAssets()
{
	// iterate the \Assets\Models directory
	for (auto & dir : Filesystem::directory_iterator(m_authoredDir / "Models")) {
		if (dir.path().has_extension()) {
			string ext = dir.path().extension().string();
			if (ext == ".fbx" || ext == ".FBX") {
				auto cmf = CMF::CreateFromFBX(dir.path(),GetStaticEM());
				cmf->Export(m_authoredDir);
			}
		}
	}
	GetStaticEM()->Save();
	//auto cmf = CMF::CreateFromFBX((m_authoredDir / "Models/Test.fbx"), GetStaticEM());
}
AssetManager::AssetManager() : m_fontSize(32)
{
	// initialize transforms

	
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

shared_ptr<CMF> g_cmf = nullptr;
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


				// TEMP **************************
				/*if (!g_cmf)
					g_cmf = CMF::CreateFromFBX("Assets/Models/Building/models-building-floor0.fbx", GetStaticEM());
				
				return g_cmf->GetModel(m_d3dDevice.Get(), distance);*/

				//****************************

				// get the LOD level
				int lod = std::min(modelAsset->LodCount - 1, modelAsset->LodSpacing > 0 ? (int)distance / modelAsset->LodSpacing : 0);
				// Get the model
				
				if (modelAsset->LODs.size() <= lod || !modelAsset->LODs[lod]) {
					//----------------------------------------------------------------
					// Cache the LOD

					
					// Load from file
					if (type == Procedural) {
						// get the path
						Filesystem::path fullPath = FullPath(path + '_' + std::to_string(lod), type, ".vbo");
						//model.reset(Model::CreateFromVBO(m_d3dDevice.Get(), fullPath.c_str()).release());
						shared_ptr<IEffect> effect;
						GetEffect(modelAsset->Effect, effect);
						model = CustomModelLoadVBO::CreateFromVBO(m_d3dDevice.Get(), fullPath.string(), effect);
						
					}
					else {
						// get the path
						Filesystem::path fullPath = m_authoredDir / "Models" / (path + ".fbx");
						model = CMF::CreateFromFBX(fullPath, GetStaticEM())->GetModel(m_d3dDevice.Get(), m_DGSLfxFactory.get() ,distance);
						//model = CustomModelLoadCMO::CreateFromCMO(m_d3dDevice.Get(), fullPath.c_str(), *m_fxFactory);
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

				if (modelAsset->RegionModels.map.size() != heightMapComp->Xsize / heightMapComp->RegionWidth) modelAsset->RegionModels.Resize(
					heightMapComp->Xsize / heightMapComp->RegionWidth,
					heightMapComp->Ysize / heightMapComp->RegionWidth);

				Rectangle sampleArea;
				// Get the region coordinates
				int x = (int)std::floor(position.x / (double)heightMapComp->RegionWidth);
				int z = (int)std::floor(position.z / (double)heightMapComp->RegionWidth);

				int lod = LOD(distance, heightMapComp->RegionWidth);
				if (modelAsset->RegionModels.map[x][z].second && modelAsset->RegionModels.map[x][z].first == lod) {
					model = modelAsset->RegionModels.map[x][z].second;
				}
				else {
					unsigned int sampleSpacing = std::pow(2, lod);
					sampleArea = Rectangle(x * heightMapComp->RegionWidth, z * heightMapComp->RegionWidth, heightMapComp->RegionWidth, heightMapComp->RegionWidth);
					auto heightMap = GetHeightMap(path, type, sampleArea, sampleSpacing);
					auto normalMap = GetNormalMap(heightMapComp->Xsize, path + "_normal", type, sampleArea, sampleSpacing);

					shared_ptr<IEffect> effect;
					GetEffect(modelAsset->Effect, effect);
					if (path == "terrain") {
						model = CreateTerrainModel(heightMapComp->Xsize,heightMapComp->RegionWidth, heightMap.get(), normalMap, sampleArea, sampleSpacing, effect);
					}
					else {
						model = CreateModelFromHeightMap(heightMapComp->RegionWidth, heightMap.get(), normalMap, sampleArea, sampleSpacing, effect);
					}
					// Cache the model;
					modelAsset->RegionModels.map[x][z].first = lod;
					modelAsset->RegionModels.map[x][z].second = model;
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
	CompileFbxAssets();
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

void AssetManager::CollectGarbage()
{
	m_proceduralEM->CollectGarbage();
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
	if (Find(type == AssetType::Authored ? m_authoredEM.get() : m_proceduralEM.get(),path, entity)) {
		// Get the heightmap component
		auto heightMapComp = entity->GetComponent<HeightMapAsset>("HeightMapAsset");
		if (heightMapComp) {
			// Load the vertex array with data.
			int xSize = sampleArea.width / sampleSpacing;
			int ySize = sampleArea.height / sampleSpacing;
			shared_ptr<HeightMap> heightMap = std::make_shared<HeightMap>(xSize,ySize);
			heightMap->area.x = sampleArea.x;
			heightMap->area.y = sampleArea.y;
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
						stream.seekg(index * sizeof(short));
						stream.read((char *)&shortBuffer, sizeof(short));

						short vertex = 0;
						memcpy(&vertex, &shortBuffer, sizeof(short));
						 
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

shared_ptr<Map<Vector3>> AssetManager::GetNormalMap(int mapWidth,string path, AssetType type, Rectangle sampleArea, int sampleSpacing)
{
	// Get the heightmap component
	// Load the vertex array with data.
	int xSize = sampleArea.width;
	int ySize = sampleArea.height;
	shared_ptr<Map<Vector3>> normalMap = std::make_shared<Map<Vector3>>(xSize, ySize);
	ifstream stream(FullPath(path, type, ".dat"), ios::binary);


	if (stream.is_open()) {
		// reads the exact bytes from the file into memory


		/*for (int vertY = 0; vertY <= ySize; vertY++) {
			for (int vertX = 0; vertX <= xSize; vertX++) {
				int globalX = vertX * sampleSpacing + sampleArea.x;
				int globalY = vertY * sampleSpacing + sampleArea.y;
				int index = Utility::posToIndex(globalX, globalY, mapWidth + 1);

				char normalBuffer[3];
				stream.seekg(index * 3 * sizeof(char));
				stream.read((char *)normalBuffer, 3 * sizeof(char));
				Vector3 normal = Vector3(float(normalBuffer[0]), float(normalBuffer[1]), float(normalBuffer[2]));
				normal.Normalize();
				normalMap->map[vertX][vertY] = normal;
			}
		}*/
		
		for (int vertY = 0; vertY <= ySize; vertY++) {
			char * normalBuffer = new char[3 * sizeof(char) * (xSize + 1) * sampleSpacing];
			int globalX = sampleArea.x;
			int globalY = vertY * sampleSpacing + sampleArea.y;
			int worldIndex = Utility::posToIndex(globalX, globalY, mapWidth + 1);

			stream.seekg(worldIndex * 3 * sizeof(char));
			stream.read(normalBuffer, 3 * sizeof(char) * (xSize + 1) * sampleSpacing);
			for (int vertX = 0; vertX <= xSize; vertX++) {
				int index = vertX * sampleSpacing * sizeof(char) * 3;
				Vector3 normal = Vector3(
					float(normalBuffer[index + 0]),
					float(normalBuffer[index + 1]),
					float(normalBuffer[index + 2])
				);
				normal.Normalize();
				normalMap->map[vertX][vertY] = normal;
			}
			delete[] normalBuffer;
		}
		
		stream.close();
		return normalMap;
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
	case Procedural: m_proceduralEM->Find(id, entity);break;
	case Authored: m_authoredEM->Find(id, entity); break;
	}
	return GetModel(entity, distance,position, type);
}

std::shared_ptr<geometry::CMF> AssetManager::GetCMF(unsigned int id, AssetType type)
{
	EntityPtr entity;
	switch (type) {
	case Procedural: m_proceduralEM->Find(id, entity);break;
	case Authored: m_authoredEM->Find(id, entity); break;
	}
	// get the path
	Filesystem::path fullPath = m_authoredDir / "Models" / (entity->GetComponent<PathID>("PathID")->Path + ".fbx");
	return CMF::CreateFromFBX(fullPath, GetStaticEM());
}

std::shared_ptr<Model> AssetManager::CreateModelFromHeightMap(
	int regionWidth,
	HeightMap * heightMap,
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
				(float)(x * sampleSpacing - regionWidth/2/*+ sampleArea.x*/),
				heightMap->map[x][z],
				(float)(z * sampleSpacing - regionWidth / 2/*+ sampleArea.y*/));
			// Upper right.
			Vector3 vertex2(
				(float)((x + 1) * sampleSpacing - regionWidth / 2/*+ sampleArea.x*/),
				heightMap->map[x + 1][z],
				(float)(z * sampleSpacing - regionWidth / 2/*+ sampleArea.y*/));
			// Bottom left.
			Vector3 vertex3(
				(float)(x * sampleSpacing - regionWidth / 2/*+ sampleArea.x*/),
				heightMap->map[x][z + 1],
				(float)((z + 1) * sampleSpacing - regionWidth / 2/*+ sampleArea.y*/));
			// Bottom right.
			Vector3 vertex4(
				(float)((x + 1) * sampleSpacing - regionWidth / 2/*+ sampleArea.x*/),
				heightMap->map[x + 1][z + 1],
				(float)((z + 1) * sampleSpacing - regionWidth / 2/*+ sampleArea.y*/));

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
			
			// Triangle 1 - Bottom left.
			vertices[index] = CreateVertex(
				XMFLOAT3(vertex3),											// position
				XMFLOAT3(normalMap->map[x][z + 1]),	// normal
				XMFLOAT2(0.f, 1.f)											// texture
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
			// Triangle 2 - Upper left.
			vertices[index] = CreateVertex(
				XMFLOAT3(vertex1),										// position
				XMFLOAT3(normalMap->map[x][z]),	// normal
				XMFLOAT2(0.f, 0.f)										// texture
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
			// Triangle 2 - Upper right.
			vertices[index] = CreateVertex(
				XMFLOAT3(vertex2),											// position
				XMFLOAT3(normalMap->map[x + 1][z]),	// normal
				XMFLOAT2(1.f, 0.f)											// texture
			);
			indices[index] = index;
			index++;
		}
	}
	return CustomModelLoadVBO::CreateFromVBO(m_d3dDevice.Get(), vertices, indices, effect);
}
std::shared_ptr<Model> AssetManager::CreateTerrainModel(int mapWidth,int regionWidth, HeightMap * heightMap, shared_ptr<Map<Vector3>> normalMap, Rectangle sampleArea, int sampleSpacing, shared_ptr<IEffect> effect)
{
	int halfRegionWidth = regionWidth / 2;
	std::shared_ptr<HeightMap> waterMap = GetHeightMap("water", AssetType::Procedural, sampleArea, sampleSpacing);
	std::shared_ptr<Map<Vector3>> waterNormalMap = GetNormalMap(mapWidth,"water_normal", AssetType::Procedural, sampleArea, sampleSpacing);
 	shared_ptr<Model> terrainModel = CreateModelFromHeightMap(regionWidth, heightMap, normalMap, sampleArea, sampleSpacing, effect);

	vector<VertexPositionNormalTangentColorTexture> vertices(6 * heightMap->width * heightMap->length);

	vector<uint16_t> indices(6 * heightMap->width * heightMap->length);

	static const float push = 0.f;

	for (int z = 0; z < heightMap->length; z++) {
		for (int x = 0; x < heightMap->width; x++) {
			const float worldX = x * sampleSpacing + sampleArea.x;
			const float worldZ = z * sampleSpacing + sampleArea.y;
			bool hasWater[4]{
				waterMap->map[x][z] > push,
				waterMap->map[x + 1][z] > push,
				waterMap->map[x][z + 1] > push,
				waterMap->map[x + 1][z + 1] > push,
			};
			DirectX::VertexPositionNormalTangentColorTexture quad[4]{
				CreateVertex(Vector3(
					(float)(x * sampleSpacing - halfRegionWidth),
					heightMap->map[x][z] > 0.f ? (waterMap->map[x][z] == 0.f ? LowestNeighbor(*waterMap,*heightMap, x, z) : waterMap->map[x][z] + heightMap->map[x][z]) : 0.f,
					(float)(z * sampleSpacing - halfRegionWidth)
				),waterNormalMap->map[x][z],Vector2::Zero),
				CreateVertex(Vector3(
					(float)((x+1) * sampleSpacing - halfRegionWidth),
					heightMap->map[x + 1][z] > 0.f ? (waterMap->map[x + 1][z] == 0.f ? LowestNeighbor(*waterMap,*heightMap, x + 1, z) : waterMap->map[x + 1][z] + heightMap->map[x + 1][z]) : 0.f,
					(float)(z * sampleSpacing - halfRegionWidth)
				),waterNormalMap->map[x+1][z],Vector2(1.f, 0.f)),
				CreateVertex(Vector3(
					(float)(x * sampleSpacing - halfRegionWidth),
					heightMap->map[x][z + 1] > 0.f ? (waterMap->map[x][z + 1] == 0.f ? LowestNeighbor(*waterMap,*heightMap, x, z + 1) : waterMap->map[x][z + 1] + heightMap->map[x][z + 1]) : 0.f,
					(float)((z + 1) * sampleSpacing - halfRegionWidth)
				),waterNormalMap->map[x][z+1],Vector2(0.f,1.f)),
				CreateVertex(Vector3(
					(float)((x+1) * sampleSpacing - halfRegionWidth),
					heightMap->map[x + 1][z + 1] > 0.f ? (waterMap->map[x + 1][z + 1] == 0.f ? LowestNeighbor(*waterMap,*heightMap, x + 1, z + 1) : waterMap->map[x + 1][z + 1] + heightMap->map[x + 1][z + 1]) : 0.f,
					(float)((z+1) * sampleSpacing - halfRegionWidth)
				),waterNormalMap->map[x+1][z+1],Vector2(1.f,1.f))
			};
			/*
			0---1
			| \ |
			2---3
			*/

			// Left triangle
			if (hasWater[0] || hasWater[3] || hasWater[2]) {
				indices.push_back(vertices.size());
				vertices.push_back(quad[0]);
				indices.push_back(vertices.size());
				vertices.push_back(quad[2]);
				indices.push_back(vertices.size());
				vertices.push_back(quad[3]);
			}
			// Right
			if (hasWater[0] || hasWater[1] || hasWater[3]) {
				indices.push_back(vertices.size());
				vertices.push_back(quad[0]);
				indices.push_back(vertices.size());
				vertices.push_back(quad[3]);
				indices.push_back(vertices.size());
				vertices.push_back(quad[1]);
			}

		}
	}
	shared_ptr<IEffect> waterEffect;
	GetEffect("Water", waterEffect);
	shared_ptr<Model> waterModel = CustomModelLoadVBO::CreateFromVBO(m_d3dDevice.Get(), vertices, indices, waterEffect);
	for (auto & mesh : waterModel->meshes) {
		for (auto & part : mesh->meshParts) {
			part->isAlpha = true;
		}
		terrainModel->meshes.push_back(mesh);
	}
	return terrainModel;
}
float AssetManager::LowestNeighbor(HeightMap & water, HeightMap & terrain, int x, int z)
{
	float minY = std::numeric_limits<float>::infinity();
	for (int i = 0; i < 8; i++) {
		int adjX = (i == 0 || i == 1 || i == 7 ? x + 1 : (i >= 3 && i <= 5 ? x - 1 : x));
		int adjZ = (i >= 1 && i <= 3 ? z + 1 : (i >= 5 && i <= 7 ? z - 1 : z));
		if (water.Bounded(adjX, adjZ)) {
			float worldY = water.map[adjX][adjZ] + terrain.map[adjX][adjZ];
			if (worldY < minY) minY = worldY;
		}
	}
	return minY;
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
bool AssetManager::Find(const unsigned int & id, AssetType & type, EntityPtr & entity)
{
	return (type == AssetType::Authored ? m_authoredEM : m_proceduralEM)->Find(id,entity);
}
bool AssetManager::Find(AssetEntityManager * assetManager, string path, EntityPtr & entity)
{
	return assetManager->TryFindByPathID(path, entity);
}
void AssetManager::AddEffect(string name, shared_ptr<IEffect> effect)
{
	m_effects.insert(std::pair<string, shared_ptr<IEffect>>(name, effect));
}
