#pragma once
#include "WorldSystem.h"
#include "CompositeModel.h"
#include "WorldDomain.h"

namespace world {
	class BuildingSystem :
		public WorldSystem
	{
	public:
		BuildingSystem(WEM *  entityManager, unsigned short updatePeriod);

		// Inherited via WorldSystem
		virtual string Name() override;
		virtual void Update(double & elapsed) override;


	public:
		//----------------------------------------------------------------
		// Create a building
		void CreateBuilding(Vector3 position, DirectX::SimpleMath::Rectangle footprint, string type);

		//----------------------------------------------------------------
		// Custom Model loader for component-based buildings
		//shared_ptr<CompositeModel> GetModel(VoxelGridModel & building, float distance);
	private:
		// Cached models
		map<unsigned int, shared_ptr<CompositeModel>> m_models;

	};

}