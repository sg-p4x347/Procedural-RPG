#include "pch.h"
#include "BuildingSystem.h"
#include "BuildingGenerator.h"
#include "AssetManager.h"
#include "JSON.h"
namespace world {
	BuildingSystem::BuildingSystem(WEM *  entityManager, unsigned short updatePeriod) : WorldSystem::WorldSystem(entityManager, updatePeriod)
	{
	}

	string BuildingSystem::Name()
	{
		return "Building";
	}

	void BuildingSystem::Update(double & elapsed)
	{
	}

	void BuildingSystem::CreateBuilding(Vector3 position, DirectX::SimpleMath::Rectangle footprint, string type)
	{
		BuildingGenerator bg;
		EM->CreateEntity(
			Position(position,Vector3(0.f,1.f, 0.71f)),
			bg.Create(footprint, JsonParser(ifstream("Config/building.json"))[type])
		);
		/*
		EntityPtr building = EM->NewEntity();
		building->AddComponent(new Components::Position(position));
		BuildingGenerator bg;
		building->AddComponent(new Components::Building(bg.Create(footprint, JsonParser(ifstream("Config/building.json"))[type])));
		*/
	}

	void BuildingSystem::CreateAdobe(Vector3 position, DirectX::SimpleMath::Rectangle footprint)
	{
		const int height = 20;
		VoxelGrid<ModelVoxel> grid(footprint.width + 1, height + 1, footprint.height + 1);
		EntityPtr floor;
		EntityPtr wall;
		EntityPtr corner;
		EntityPtr wallCap;
		EntityPtr cornerCap;
		EntityPtr doorFrameBottom;
		EntityPtr doorFrameTop;
		if (AssetManager::Get()->GetStaticEM()->TryFindByPathID("adobe_wall",wall)
			&& AssetManager::Get()->GetStaticEM()->TryFindByPathID("adobe_corner", corner)
			//&& AssetManager::Get()->GetStaticEM()->TryFindByPathID("adobe_wall_cap", wallCap)
			//&& AssetManager::Get()->GetStaticEM()->TryFindByPathID("adobe_corner_cap", cornerCap)
			//&& AssetManager::Get()->GetStaticEM()->TryFindByPathID("adobe_door_frame_bottom", doorFrameBottom)
			//&& AssetManager::Get()->GetStaticEM()->TryFindByPathID("adobe_door_frame_top", doorFrameTop)
			//&& AssetManager::Get()->GetStaticEM()->TryFindByPathID("adobe_floor", floor)) 
		) {
			Int3 start(0, 0, 0);
			Int3 end(footprint.width, 0, footprint.height);
			// Floor
			//AddUniform(grid, floor->ID(), Transforms::None, start, end);
			// Walls
			AddPerimeter(grid, wall->ID(), corner->ID(), Int3(start.x,0,start.z), Int3(end.x,height-1,end.z));
			// Wall Caps
			//AddPerimeter(grid, wallCap->ID(), cornerCap->ID(), Int3(start.x, height, start.z), Int3(end.x, height, end.z));
		
			EM->CreateEntity(world::Position(position), world::VoxelGridModel(grid));
		}
		
	}

	void BuildingSystem::AddUniform(Grid & grid, AssetID asset, TransformID transform, Int3 start, Int3 end)
	{
		for (int x = start.x; x <= end.x; x++) {
			for (int y = start.y; y <= end.y; y++) {
				for (int z = start.z; z <= end.z; z++) {
					grid[x][y][z].AddComponent(asset, transform);
				}
			}
		}
		
	}

	void BuildingSystem::AddPerimeter(Grid & grid, AssetID sideAsset,AssetID cornerAsset, Int3 start, Int3 end)
	{
		for (int x = start.x; x <= end.x; x++) {
			for (int z = start.y; z <= end.z; z++) {
				int zLength = end.z - start.z;
				int unitZ = z == start.z ? -1 : (z - start.z) / zLength;
				int xLength = end.x - start.z;
				int unitX = x == start.x ? -1 : (x - start.x) / xLength;
				for (int y = start.y; y <= end.y; y++) {
					/*
					^
					|
					+X
							  1
					 +--------+--------+
					 |		  |		   |
					 |		  |		   |
					 |		  |		   |
					2+--------*--------+0
					 |		  |		   |
					 |		  |		   |
					 |		  |		   |
					 +--------+--------+
							  3
					+Z --->
					*/	
					auto & voxel = grid[x][y][z];
					if (abs(unitX) == 1 && abs(unitZ) == 1) {
						if (unitZ == 1) {
							if (unitX == 1) {
								voxel.AddComponent(cornerAsset, Transforms::None);
							}
							else {
								voxel.AddComponent(cornerAsset, Transforms::Rotate270);
							}
						}
						else {
							if (unitX == 1) {
								voxel.AddComponent(cornerAsset, Transforms::Rotate90);
							}
							else {
								voxel.AddComponent(cornerAsset, Transforms::Rotate180);
							}
						}
					}
					else if (abs(unitZ) == 1) {
						voxel.AddComponent(sideAsset, 2 - (unitZ + 1));
					}
					else if (abs(unitX) == 1) {
						voxel.AddComponent(sideAsset, 3 - (unitX + 1));
					}
				}
			}
		}
	}

	//shared_ptr<CompositeModel> BuildingSystem::GetModel(VoxelGridModel & building, float distance)
	//{
	//	// Search the cache
	//	if (m_models.find(building.ID) != m_models.end()) {
	//		return m_models[building.ID];
	//	}
	//	// Construct the model from component models
	//	shared_ptr<CompositeModel> model = make_shared<CompositeModel>();
	//	VoxelGrid<ModelVoxel> & voxels = building.Voxels;
	//	for (int x = 0; x < voxels.GetXsize(); x++) {
	//		for (int z = 0; z < voxels.GetZsize(); z++) {
	//			BuildingVoxel & voxel = voxels.map[x][z];
	//			XMMATRIX voxelTranslate = XMMatrixTranslation(x, 0.f, z);
	//			//----------------------------------------------------------------
	//			// Floors
	//			for (int i = 0; i < voxel.GetFloorCount(); i++) {
	//				unsigned int assetID = voxel.GetFloors()[i];
	//				if (assetID) {
	//					shared_ptr<DirectX::Model> floorComp = AssetManager::Get()->GetModel(assetID, distance);
	//					if (floorComp) {
	//						for (auto & mesh : floorComp->meshes) {
	//							model->AddMesh(mesh, voxelTranslate);
	//						}
	//					}
	//					//Utility::Concat(model->meshes, floorComp->meshes);
	//				}
	//			}
	//			//----------------------------------------------------------------
	//			// Walls
	//			for (int i = 0; i < voxel.GetWallCount(); i++) {
	//				unsigned int assetID = voxel.GetWalls()[i];
	//				if (assetID) {
	//					shared_ptr<DirectX::Model> wallComp = AssetManager::Get()->GetModel(assetID, distance);
	//					if (wallComp) {
	//						float yaw = ((i + 1) % 4)  * HALF_PI;
	//						XMMATRIX rotation = XMMatrixRotationNormal(Vector3::UnitY, yaw);
	//						XMMATRIX translate = XMMatrixTranslation(-0.5f, 0.f, 0.f);
	//						XMMATRIX transform = XMMatrixMultiply(translate, rotation);
	//						// Move to the voxel location in the map
	//						transform = XMMatrixMultiply(transform, voxelTranslate);

	//						for (auto & mesh : wallComp->meshes) {
	//							model->AddMesh(mesh, transform);
	//						}
	//						//Utility::Concat(model->meshes, wallComp->meshes);
	//					}
	//				}
	//			}
	//			//----------------------------------------------------------------
	//			// Corners
	//			for (int i = 0; i < voxel.GetCornerCount(); i++) {
	//				unsigned int assetID = voxel.GetCorners()[i];
	//				if (assetID) {
	//					shared_ptr<DirectX::Model> wallComp = AssetManager::Get()->GetModel(assetID, distance);
	//					if (wallComp) {
	//						float yaw = (i % 4)  * HALF_PI;
	//						XMMATRIX rotation = XMMatrixRotationNormal(Vector3::UnitY, yaw);
	//						// Move to the voxel location in the map
	//						XMMATRIX transform = XMMatrixMultiply(rotation, voxelTranslate);

	//						for (auto & mesh : wallComp->meshes) {
	//							model->AddMesh(mesh, transform);
	//						}
	//						//Utility::Concat(model->meshes, wallComp->meshes);
	//					}
	//				}
	//			}
	//		}
	//	}
	//	// Cache this model
	//	m_models.insert(std::make_pair(building.ID, model));
	//	return model;
	//}
}