#pragma once
#include "WorldSystem.h"
#include "CompositeModel.h"
#include "WorldDomain.h"

namespace world {
	class BuildingSystem :
		public WorldSystem
	{
	public:
		BuildingSystem(WEM *  entityManager, vector<string> components, unsigned short updatePeriod);

		// Inherited via WorldSystem
		virtual string Name() override;
		virtual void Update(double & elapsed) override;
	public:
		//----------------------------------------------------------------
		// Create a building
		void CreateBuilding(Vector3 position, Rectangle footprint, string type);

		//----------------------------------------------------------------
		// Custom Model loader for component-based buildings
		shared_ptr<CompositeModel> GetModel(EntityPtr building, float distance);
	private:
		// Cached models
		map<unsigned int, shared_ptr<CompositeModel>> m_models;

	};

}