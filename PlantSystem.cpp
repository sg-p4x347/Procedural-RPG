#include "pch.h"
#include "PlantSystem.h"
#include "SystemManager.h"
#include "TreeGenerator.h"
#include "AssetManager.h"
#include "TerrainSystem.h"
#include "Inventory.h"
#include "ItemSystem.h"
#include "Geometry.h"
#include "CMF.h"
#include "Game.h"
#include <list>
namespace world {
	PlantSystem::PlantSystem(SystemManager * systemManager, WEM * entityManager, unsigned short updatePeriod) : 
		WorldSystem::WorldSystem(entityManager, updatePeriod),
		m_entities(EM->NewEntityCache<Position,Plant>()),
		SM(systemManager)
	{
		
	}

	string PlantSystem::Name()
	{
		return "Plant";
	}

	void PlantSystem::Update(double & elapsed)
	{
		// update plants according to their dna
		if (Game::Get().KeyboardTracker.IsKeyPressed(DirectX::Keyboard::Keys::V)) {
			
		//}
		//if (Game::Get().KeyboardTracker.IsKeyPressed(DirectX::Keyboard::Keys::C)) {
			auto plant = std::make_shared<Plant>(100.f, 100.f);
			AssetID asset = AssetManager::Get()->AddModel(plant->model);
			EM->CreateEntity(Position(EM->PlayerPos()), Model(asset, AssetType::Procedural));

			plant->seed = std::make_shared<Plant::Seed>(*plant, Matrix::Identity, 0.01f);
			Plant::GrowNewComponent addRoot;
			addRoot.type = Plant::ComponentTypes::RootComponent;
			addRoot.componentIndex = 0;
			plant->AddAction(addRoot);
			Plant::Grow growRoot;
			growRoot.componentIndex = 1;
			growRoot.radius = 0.5f;
			plant->AddAction(growRoot);
			int levels = 7;
			for (int i = 0; i < levels; i++) {
				Plant::GrowNewComponent addStem;
				addStem.type = Plant::ComponentTypes::StemComponent;
				addStem.componentIndex = i+1;
				addStem.yawPitchRoll.y = 0.25f;
				addStem.yawPitchRoll.x = XM_PI * 0.628f;
				addStem.position = 1.f;
				plant->AddAction(addStem);
				addStem.yawPitchRoll.y = -0.25f;
				plant->AddAction(addStem);

				Plant::Grow growStem;
				growStem.componentIndex = i+2;
				growStem.radius = 0.02f * (levels - i);
				plant->AddAction(growStem);
			}
			plants.push_back(plant);

			UpdateGeneticPlants(Vector3::Down, plants);
		}
		for (auto & entity : m_entities) {

		}
	}

	void PlantSystem::Generate()
	{
		
		auto terrainSystem = SM->GetSystem<TerrainSystem>("Terrain");
		
		JsonParser config(std::ifstream("config/plants.json"));
		if (config["trees"].To<bool>()) {
			GenerateTreeModels();
			GenerateTreeEntities(*(terrainSystem->m_terrainMap), *(terrainSystem->m_waterMap));
		}
		if (config["grass"].To<bool>()) {
			GenerateGrassEntities(*(terrainSystem->m_terrainMap), *(terrainSystem->m_waterMap));
		}
		//GenerateGeneticPlants();
	}

	void PlantSystem::GenerateGeneticPlants()
	{
		JsonParser config(std::ifstream("config/plants.json"));
		int iterations = config["iterations"].To<int>();
		int minPopulation = config["minPopulation"].To<int>();
		
		
		for (int i = 0; i < iterations; i++) {
			// generate offspring from current plants
			GenerateOffspring(plants);
			// ensure that there are at least minPopulation plants
			
			// update plants according to their dna
			UpdateGeneticPlants(Vector3::Down, plants);
			
		}
	}

	void PlantSystem::GenerateOffspring(std::list<shared_ptr<Plant>> & plants)
	{
	}

	void PlantSystem::UpdateGeneticPlants(Vector3 lightDirection, std::list<shared_ptr<Plant>> & plants)
	{
		vector<shared_ptr<Plant>> dead;
		for (auto & plant : plants) {
			// Get external resources
			Plant::ExternalResource external;
			external.Temperature = 20.f;
			// Get the next action
			while (plant->DnaCursor < plant->DNA.size()) {
				auto & action = plant->DNA[plant->DnaCursor];
				shared_ptr<Plant::Grow> grow = dynamic_pointer_cast<Plant::Grow>(action);
				shared_ptr<Plant::GrowNewComponent> growNewComponent = dynamic_pointer_cast<Plant::GrowNewComponent>(action);
				shared_ptr<Plant::CollectWaterAction> collectWater = dynamic_pointer_cast<Plant::CollectWaterAction>(action);
				shared_ptr<Plant::CreateSugarAction> createSugar = dynamic_pointer_cast<Plant::CreateSugarAction>(action);
				if (grow) {
					vector<Plant::PlantComponent*> components;
					plant->seed->NthComponents(grow->componentIndex, components);
					for (auto & component : components) {
						component->Grow(grow->radius);
					}
					UpdateModel(*plant);
				} else if (createSugar) {
					external.CO2 = 100.f;
					// calculate how much external light is available
					external.Light = CalculateLight(lightDirection,*plant);
					plant->CreateSugar(external);
				}
				else if (collectWater) {
					// calculate how much external water is available
					// Based on a half sphere volume
					for (auto & root : plant->Roots) {
						external.Water += root->GetMass() * 1.f;
					}
					plant->CollectWater(external);
				}
				else if (growNewComponent) {
					vector<Plant::PlantComponent*> components;
					plant->seed->NthComponents(growNewComponent->componentIndex, components);
					for (auto & parentComponent : components) {
						shared_ptr<Plant::PlantComponent> component;
						Matrix transform = Matrix::CreateFromYawPitchRoll(
							growNewComponent->yawPitchRoll.x,
							growNewComponent->yawPitchRoll.y,
							growNewComponent->yawPitchRoll.z
						) * Matrix::CreateTranslation(Vector3(0.f,growNewComponent->position,0.f));

						switch (growNewComponent->type) {
						case Plant::ComponentTypes::LeafComponent:
							component = shared_ptr<Plant::PlantComponent>(new Plant::Leaf(parentComponent,transform));
							plant->Leaves.push_back(dynamic_pointer_cast<Plant::Leaf>(component));
							break;
						case Plant::ComponentTypes::StemComponent:
							component = shared_ptr<Plant::PlantComponent>(new Plant::Stem(parentComponent, transform,1.f,0.001f));
							plant->Stems.push_back(dynamic_pointer_cast<Plant::Stem>(component));
							break;
						case Plant::ComponentTypes::RootComponent:
							component = shared_ptr<Plant::PlantComponent>(new Plant::Root(parentComponent, transform, 0.001f));
							plant->Roots.push_back(dynamic_pointer_cast<Plant::Root>(component));
							break;
						}
						parentComponent->Children.push_back(component);
					}
					UpdateModel(*plant);
				}
				plant->DnaCursor++;
			}
			// Operational cost
			plant->Sugar -= 0.1f;

			// Check for death status
			if (plant->Sugar < 0.f) {
				dead.push_back(plant);
			}
		}
		// remove the dead plants
		for (auto & plant : dead) {
			plants.remove(plant);
		}
	}

	float PlantSystem::CalculateLight(Vector3 lightDirection, Plant & plant)
	{
		static const float leafRadius = 0.1f;
		float light = 0.f;
		for (auto & leaf : plant.Leaves) {
			float effectiveness = Vector3::Transform(Vector3::Up, leaf->Transform).Dot(-lightDirection) / 3.f;
			if (effectiveness > 0.f) {
				// Create a grid of rays to cast out from the leaf
				vector<Vector3> raySources = leaf->GetVertices();
				for (auto & raySource : raySources) {
					for (auto & otherLeaf : plant.Leaves) {
						if (otherLeaf != leaf && geometry::RayIntersectTriangle(raySource, -lightDirection, raySources[0], raySources[1], raySources[2]))
							goto nextRay;
					}
					for (auto & stem : plant.Stems) {
						auto start = stem->GetStart();
						auto axis = stem->GetEnd() - start;
						if (geometry::RayIntersectCylinder(raySource, -lightDirection, start, axis, stem->Radius))
							goto nextRay;
					}
					for (auto & root : plant.Roots) {
						auto center = root->GetCenter();
						auto normal = root->GetNormal();
						if (geometry::RayIntersectHalfSphere(raySource, -lightDirection, center, root->Radius, normal))
							goto nextRay;
					}
					// No collisions detected, count this ray
					light += effectiveness;

					nextRay:;
				}
			}
		}
		return light;
	}

	void PlantSystem::UpdateModel(Plant & plant)
	{
		
		TopologyCruncher tc;
		auto lod = plant.model->GetLOD();
		
		auto & mesh = lod->GetMeshes()[0];
		mesh->GetParts().clear();
		for (auto & stem : plant.Stems) {
			GenerateTopologyRecursive(stem, tc);
			auto && part = tc.CreateMeshPart();
			part.SetMaterial(plant.model->GetMaterial("wood"));
			mesh->AddPart(part);
		}
		lod->ModelChanged();
	}

	void PlantSystem::GenerateTopologyRecursive(shared_ptr<Plant::Stem> stem, TopologyCruncher & tc)
	{
		vector<Vector3> path{ stem->GetStart(),stem->GetEnd() };
		tc.Tube(path, [=](float & t) {
			return stem->Radius * 2.f;
		},
			1,
			10,
			PathType::LinearPath
			);
	}

	void PlantSystem::GenerateTreeModels()
	{

		TreeGenerator tg;
		AssetManager::Get()->AddModel(tg.Generate(TreeType::SingleAxis), AssetType::Procedural);
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
		Utility::OutputLine("\nGenerating Trees...");
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
						Vector3 pos(x, terrain.Height(x, z)- 0.2f, z);
						Vector3 rot(Utility::randWithin(-0.05f, 0.05f), Utility::randWithin(0.f, XM_2PI), Utility::randWithin(-0.05f, 0.05f));
						NewTree(pos, rot);
					}
				}
			}
		}
		Utility::OutputLine("Finished");
	}
	void PlantSystem::NewTree(DirectX::SimpleMath::Vector3 & position, Vector3 & rotation)
	{
		EntityPtr treeAsset;
		if (AssetManager::Get()->GetProceduralEM()->TryFindByPathID("tree", treeAsset)) {
			EM->CreateEntity(
				Position(position,rotation),
				Model(treeAsset->ID(), AssetType::Procedural),
				Collision(treeAsset->ID(),AssetType::Procedural)
			);
		}
		
		//entity->AddComponent(
		//	new Components::Position(position, rotation));
		//entity->AddComponent(
		//	new Components::Tag("Tree"));
		//entity->AddComponent(
		//	new Components::Model("Tree", AssetType::Procedural));
		//entity->AddComponent(
		//	new Components::Inventory());
		//entity->AddComponent(
		//	new Components::Action(Vector3(1.f, 10.f, 1.f), EventTypes::Resource_Aquire, entity->ID()));

		//// Fill the tree's inventory with logs
		//shared_ptr<Components::Inventory> inv = entity->GetComponent<Components::Inventory>("Inventory");
		//auto itemSys = SM->GetSystem<ItemSystem>("Item");
		//itemSys->AddItem(inv, "Logs", 3);
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

	void PlantSystem::GenerateGrassEntities(HeightMap & terrain, Map<WaterCell> & water)
	{
		Utility::OutputLine("\nGenerating Grass...");
		static const float density = 0.5f;
		for (int x = 0; x <= terrain.width; x++) {
			for (int z = 0; z <= terrain.length; z++) {
				if (water.map[x][z].Water == 0.f) {
					float height = terrain.Height(x, z);
					float probability = density;
					probability *= TreeGradientProbability(terrain.GradientAngle(x, z));
					probability *= TreeElevationProbability(height);
					//probability = std::round(probability);
					if (probability > 0.0001f && Utility::Chance(probability)) {
						Vector3 pos(x + Utility::randWithin(-0.5f,0.5f), height - Utility::randWithin(0.f, 0.5f), z + Utility::randWithin(-0.5f, 0.5f));
						Vector3 rot(0.f, Utility::randWithin(0.f, XM_2PI), 0.f);
						NewGrass(pos, rot);
					}
				}
			}
		}
		Utility::OutputLine("Finished");
	}

	void PlantSystem::NewGrass(DirectX::SimpleMath::Vector3 & position, Vector3 & rotation)
	{
		EntityPtr grassAsset;
		if (AssetManager::Get()->GetStaticEM()->TryFindByPathID("Grass", grassAsset)) {
			EM->CreateEntity(
				Position(position, rotation),
				Model(grassAsset->ID(), AssetType::Authored)
			);
		}
	}
}