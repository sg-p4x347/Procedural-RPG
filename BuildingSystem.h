#pragma once
#include "WorldSystem.h"
#include "WorldDomain.h"
#include "VoxelGrid.h"
#include "ModelVoxel.h"
#include "Geometry.h"
using geometry::Int3;
namespace world {
	class BuildingSystem :
		public WorldSystem
	{
	public:
		typedef VoxelGrid<ModelVoxel> Grid;
		BuildingSystem(WEM *  entityManager, unsigned short updatePeriod);

		// Inherited via WorldSystem
		virtual string Name() override;
		virtual void Update(double & elapsed) override;


	public:
		//----------------------------------------------------------------
		// Create a building
		void CreateBuilding(Vector3 position, DirectX::SimpleMath::Rectangle footprint, string type);
		void CreateAdobe(Vector3 position, DirectX::SimpleMath::Rectangle footprint);
		//----------------------------------------------------------------
		// Custom Model loader for component-based buildings
		//shared_ptr<CompositeModel> GetModel(VoxelGridModel & building, float distance);
	private:
		
		// Cached models
		//map<unsigned int, shared_ptr<CompositeModel>> m_models;
		//----------------------------------------------------------------
		// Helpers
		void AddUniform(Grid & grid,AssetID asset, TransformID transform, Int3 start, Int3 end);
		void AddPerimeter(Grid & grid, AssetID sideAsset, AssetID cornerAsset, Int3 start, Int3 end);
		
	};

}