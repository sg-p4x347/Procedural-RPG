#pragma once
#include "WorldComponent.h"
#include "MeshPart.h"
namespace world {
	class Plant :
		public WorldComponent
	{
	public:
		// Component classes
		struct ExternalResource {
			float Light;
			float Temperature;
			float Water;
			float CO2;
		};
		enum ComponentTypes {
			Leaf,
			Stem,
			Root,
			Seed
		};
		struct PlantComponent {
			PlantComponent(Plant & plant);
			vector<shared_ptr<PlantComponent>> Children;
			shared_ptr<PlantComponent> Parent;
			Plant & ParentPlant;
			Matrix Transform;
			shared_ptr<geometry::MeshPart> MeshPart;
			

			virtual void Convert(ExternalResource & external) = 0;
			void NthComponents(int n, vector<shared_ptr<PlantComponent>> & components);
			
		};
		struct Leaf : public PlantComponent {
			virtual void Update(ExternalResource & external);
		};
		struct Stem : public PlantComponent {
			virtual void Update(ExternalResource & external);
		};
		struct Root : public PlantComponent {
			virtual void Update(ExternalResource & external);
		};
		struct Seed : public PlantComponent {
			virtual void Update(ExternalResource & external);
		};
		//----------------------------------------------------------------
		// Genetic Algorithm Actions
		enum ActionTypes {
			GrowAction,
			GrowNewComponentAction,
			CollectWaterAction,
			CreateSugarAction
		};
		struct Grow {
			int componentIndex;
			float radius;
		};
		struct GrowNewComponent {
			ComponentTypes type;
			Vector3 position;
			Vector3 rotation;
		};
		struct CollectWater {
			float amount;
		};
		struct CreateSugar {
			float amount;
		};
		union Action {
			ActionTypes type;
			Grow grow;
			GrowNewComponent growNewComponent;
			CollectWater collectWater;
			CreateSugar createSugar;
		};
	public:

		Plant();
		float AvailableCapacity();
		float Effectiveness(float temperature);
	public:
		// Genetic Data
		vector<Action> DNA;
		int DnaCursor;
		float OptimalTemperature;
		float TemperatureTolerance;
		// Internal resources
		float Sugar;	// Acts as action currency
		float Water;	// Environmental
		float Volume;	// Acts as a limit to how much sugar and water can be held
		// Physical model
		shared_ptr<PlantComponent> Base;
	};
}
