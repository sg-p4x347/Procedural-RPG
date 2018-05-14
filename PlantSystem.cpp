#include "pch.h"
#include "PlantSystem.h"
#include "SystemManager.h"
#include "TreeGenerator.h"
#include "AssetManager.h"
#include "TerrainSystem.h"
#include "Inventory.h"
#include "ItemSystem.h"
PlantSystem::PlantSystem(SystemManager * systemManager, unique_ptr<WorldEntityManager>& entityManager, vector<string>& components, unsigned short updatePeriod) : WorldSystem::WorldSystem(entityManager,components,updatePeriod),
SM(systemManager)
{
	
}

string PlantSystem::Name()
{
	return "Plant";
}

void PlantSystem::Update(double & elapsed)
{
}

void PlantSystem::Generate()
{
	GenerateTreeModels();
	auto terrainSystem = SM->GetSystem<TerrainSystem>("Terrain");
	GenerateTreeEntities(*(terrainSystem->TerrainMap),*(terrainSystem->WaterMap));
}

void PlantSystem::GenerateTreeModels()
{
	
	TreeGenerator tg;
	static const int lodCount = 4;
	static const int lodSpacing = 8;
	vector<shared_ptr<Components::VBO<VertexPositionNormalTangentColorTexture>>> vbos;
	for (int lod = 0; lod < lodCount; lod++) {

	
		vbos.push_back(
			std::make_shared<Components::VBO<VertexPositionNormalTangentColorTexture>>(tg.Generate(TreeType::SingleAxis,lod))
		);
		//Components::PositionNormalTextureVBO * vbo = new Components::PositionNormalTextureVBO(tc.CreateVBO());
		
	}
	AssetManager::Get()->GetProceduralEM()->CreateModel("Tree",vbos, lodSpacing,"Wood");
	//{
	//	EntityPtr test = EM->NewEntity();
	//	test->AddComponent(new Components::Model("Tree", "Default", true, false));
	//	test->AddComponent(new Components::Position(Vector3(10, 0, 10)));
	//	EM->AddEntityToRegion(test);
	//	/*auto vbo = test->GetComponent<Components::PositionNormalTextureVBO>("PositionNormalTextureVBO");
	//	vbo->Effect = "Terrain";
	//	vbo->LODchanged = true;
	//	vp->ImportVBO(Filesystem::path("test.vbo"), *vbo);*/

	//}

	AssetManager::Get()->GetProceduralEM()->Save();
}

void PlantSystem::GenerateTreeEntities(HeightMap & terrain, Map<WaterCell> & water)
{
	Utility::OutputLine("Generating Trees...");
	static const float density = 0.01f;
	/*for (int regionX = 0; regionX < m_width / m_regionWidth; regionX++) {
	for (int regionZ = 0; regionZ < m_width / m_regionWidth; regionZ++) {*/
	for (int x = 0; x <= terrain.width; x++) {
		for (int z = 0; z <= terrain.length; z++) {
			if (water.map[x][z].Water == 0.f) {

				float probability = density;
				probability *= TreeGradientProbability(terrain.GradientAngle(x, z));
				probability *= TreeElevationProbability(terrain.Height(x, z));
				//probability = std::round(probability);
				if (probability > 0.0001f && Utility::Chance(probability)) {
					Vector3 pos(x, terrain.Height(x, z), z);
					Vector3 rot(0.f, Utility::randWithin(0.f, XM_2PI), 0.f);
					NewTree(pos, rot);
				}
			}
		}
	}
	Utility::OutputLine("Finished");
}
void PlantSystem::NewTree(DirectX::SimpleMath::Vector3 & position, Vector3 & rotation)
{
	EntityPtr entity = EM->NewEntity();

	entity->AddComponent(
		new Components::Position(position, rotation));
	entity->AddComponent(
		new Components::Tag("Tree"));
	entity->AddComponent(
		new Components::Model("Tree", AssetType::Procedural));
	entity->AddComponent(
		new Components::Inventory());
	entity->AddComponent(
		new Components::Action(Vector3(1.f, 10.f, 1.f),EventTypes::Resource_Aquire,entity->ID()));

	// Fill the tree's inventory with logs
	shared_ptr<Components::Inventory> inv = entity->GetComponent<Components::Inventory>("Inventory");
	auto itemSys = SM->GetSystem<ItemSystem>("Item");
	itemSys->AddItem(inv, "Logs", 3);
	//new Components::Model("Tree", "Default")
	// Add action node
	//SM->GetSystem<ActionSystem>("Action")->CreateAction(position, Vector3(1.f, 10.f, 1.f), EventTypes::Action_GatherWood, entity->ID());
}

float PlantSystem::TreeGradientProbability(float gradient)
{
	static const float maxGradient = Utility::DegToRad(45);
	return Utility::SigmoidDecay(gradient, maxGradient);
}

float PlantSystem::TreeElevationProbability(float elevation)
{
	static const float maxElevation = 512.f;
	return Utility::SigmoidDecay(elevation, maxElevation, 0.1f);
}
