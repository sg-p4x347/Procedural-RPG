#pragma once
#include "WorldSystem.h"
#include "WorldDomain.h"
namespace world {
	class MovementControllerSystem :
		public world::WorldSystem
	{
	public:
		MovementControllerSystem(world::WEM * entityManager);
		~MovementControllerSystem();

		// Inherited via WorldSystem
		virtual string Name() override;
		virtual void Update(double & elapsed) override;
	private:
		WorldEntityCache<WEM::RegionType, Collision, Movement, MovementController> m_entities;
	private:
		void RegisterHandlers();
	};
}
