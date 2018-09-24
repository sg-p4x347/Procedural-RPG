#pragma once
#include "WorldSystem.h"
#include "IEventManager.h"
#include "WorldDomain.h"
/*----------------------------------------------------------------
 Purpose:
 Provide standard operations on world action nodes. Designed to be
 utilized by other systems for the creation and life-cycle of
 action node entities
 ----------------------------------------------------------------*/
class SystemManager;
namespace world {
	class ActionSystem :
		public WorldSystem
	{
	public:
		ActionSystem(SystemManager *, WEM * entityManager, unsigned short updatePeriod);

		// Inherited via WorldSystem
		virtual string Name() override;
		virtual void Update(double & elapsed) override;

		void Check();
		void CreateAction(Vector3 position, Vector3 size, EventTypes event, EntityPtr targetEntity);
	private:

		SystemManager * SM;
		//----------------------------------------------------------------
		// Utility
		Vector3 GetPlayerLookRay();
		bool CanInteract(EntityPtr & actionNode);
	};

}