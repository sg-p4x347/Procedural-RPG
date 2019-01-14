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
			LeafComponent,
			StemComponent,
			RootComponent
		};
		struct PlantComponent {
			PlantComponent(Plant & plant, Matrix & transform);
			vector<shared_ptr<PlantComponent>> Children;
			shared_ptr<PlantComponent> Parent;
			Plant & ParentPlant;
			Matrix Transform;

			void NthComponents(int n, vector<shared_ptr<PlantComponent>> & components);
			virtual void Grow(float radius) = 0;
			virtual float GetMass() = 0;
		};
		struct Leaf : public PlantComponent {
			Leaf(Plant & plant, Matrix & transform);
			void Grow(float radius) override;
			float GetMass() override;
		};
		struct Stem : public PlantComponent {
			Stem(Plant & plant, Matrix & transform, float length, float radius);
			float Length;
			float Radius;
			void Grow(float radius) override;
			float GetMass() override;
		};
		struct Root : public PlantComponent {
			float Radius;
			void Grow(float radius) override;
			float GetMass() override;
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
			int componentIndex;
			ComponentTypes type;
			float position;
			float roll;
			Vector3 direction;
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
		void CreateSugar(ExternalResource & external);
		void CollectWater(ExternalResource & external);
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
