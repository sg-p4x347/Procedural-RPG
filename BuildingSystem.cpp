#include "pch.h"
#include "BuildingSystem.h"
#include "BuildingGenerator.h"
#include "AssetManager.h"
BuildingSystem::BuildingSystem(WorldEntityManager *  entityManager, vector<string> components, unsigned short updatePeriod) : WorldSystem::WorldSystem(entityManager,components,updatePeriod)
{

}

string BuildingSystem::Name()
{
	return "Building";
}

void BuildingSystem::Update(double & elapsed)
{
}

void BuildingSystem::CreateBuilding(Vector3 position, Rectangle footprint, string type)
{
	EntityPtr building = EM->NewEntity();
	building->AddComponent(new Components::Position(position));
	BuildingGenerator bg;
	building->AddComponent(new Components::Building(bg.Create(footprint, JsonParser(ifstream("Config/building.json"))[type])));
}

shared_ptr<CompositeModel> BuildingSystem::GetModel(EntityPtr building, float distance)
{
	// Search the cache
	if (m_models.find(building->ID()) != m_models.end()) {
		return m_models[building->ID()];
	}
	// Construct the model from component models
	shared_ptr<CompositeModel> model = make_shared<CompositeModel>();
	auto buildingComp = building->GetComponent<Components::Building>("Building");
	Map<BuildingVoxel> & voxels = buildingComp->Voxels;
	for (int x = 0; x <= voxels.width; x++) {
		for (int z = 0; z <= voxels.length; z++) {
			BuildingVoxel & voxel = voxels.map[x][z];
			XMMATRIX voxelTranslate = XMMatrixTranslation(x, 0.f, z);
			//----------------------------------------------------------------
			// Floors
			for (int i = 0; i < voxel.GetFloorCount(); i++) {
				unsigned int assetID = voxel.GetFloors()[i];
				if (assetID) {
					shared_ptr<Model> floorComp = AssetManager::Get()->GetModel(assetID, distance);
					if (floorComp) {
						for (auto & mesh : floorComp->meshes) {
							model->AddMesh(mesh, voxelTranslate);
						}
					}
					//Utility::Concat(model->meshes, floorComp->meshes);
				}
			}
			//----------------------------------------------------------------
			// Walls
			for (int i = 0; i < voxel.GetWallCount(); i++) {
				unsigned int assetID = voxel.GetWalls()[i];
				if (assetID) {
					shared_ptr<Model> wallComp = AssetManager::Get()->GetModel(assetID, distance);
					if (wallComp) {
						float yaw = ((i + 1) % 4)  * HALF_PI;
						XMMATRIX rotation = XMMatrixRotationNormal(Vector3::UnitY, yaw);
						XMMATRIX translate = XMMatrixTranslation(-0.5f, 0.f, 0.f);
						XMMATRIX transform = XMMatrixMultiply(translate, rotation);
						// Move to the voxel location in the map
						transform = XMMatrixMultiply(transform, voxelTranslate);

						for (auto & mesh : wallComp->meshes) {
							model->AddMesh(mesh, transform);
						}
						//Utility::Concat(model->meshes, wallComp->meshes);
					}
				}
			}
			//----------------------------------------------------------------
			// Corners
			for (int i = 0; i < voxel.GetCornerCount(); i++) {
				unsigned int assetID = voxel.GetCorners()[i];
				if (assetID) {
					shared_ptr<Model> wallComp = AssetManager::Get()->GetModel(assetID, distance);
					if (wallComp) {
						float yaw = (i % 4)  * HALF_PI;
						XMMATRIX rotation = XMMatrixRotationNormal(Vector3::UnitY, yaw);
						// Move to the voxel location in the map
						XMMATRIX transform = XMMatrixMultiply(rotation, voxelTranslate);

						for (auto & mesh : wallComp->meshes) {
							model->AddMesh(mesh, transform);
						}
						//Utility::Concat(model->meshes, wallComp->meshes);
					}
				}
			}
		}
	}
	// Cache this model
	m_models.insert(std::make_pair(building->ID(), model));
	return model;
}
