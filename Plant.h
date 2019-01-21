#pragma once
#include "WorldComponent.h"
#include "MeshPart.h"
#include "CMF.h"
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
			PlantComponent(Plant & plant, Matrix transform);
			vector<shared_ptr<PlantComponent>> Children;
			shared_ptr<PlantComponent> Parent;
			Plant & ParentPlant;
			Matrix Transform;
			Matrix WorldMatrix;

			void NthComponents(int n, vector<shared_ptr<PlantComponent>> & components);
			virtual void Grow(float radius) = 0;
			virtual float GetMass() = 0;
		};
		struct Leaf : public PlantComponent {
			Leaf(Plant & plant, Matrix transform);
			
			vector<Vector3> GetVertices();
			void Grow(float radius) override;
			float GetMass() override;
		};
		struct Stem : public PlantComponent {
			Stem(Plant & plant, Matrix transform, float length, float radius);
			float Length;
			float Radius;
			
			Vector3 GetStart();
			Vector3 GetEnd();
			void Grow(float radius) override;
			float GetMass() override;
		};
		struct Root : public PlantComponent {
			Root(Plant & plant, Matrix transform, float radius);
			float Radius;

			Vector3 GetCenter();
			Vector3 GetNormal();
			void Grow(float radius) override;
			float GetMass() override;
		};
		struct Seed : public PlantComponent {
			Seed(Plant & plant, Matrix transform, float radius);
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
			Vector3 yawPitchRoll;
		};
		struct CollectWater {
			float amount;
		};
		struct CreateSugar {
			float amount;
		};
		union Action {
			Action(){}
			ActionTypes type;
			Grow grow;
			GrowNewComponent growNewComponent;
			CollectWater collectWater;
			CreateSugar createSugar;
		};
	public:
		Plant(float sugar = 0.f, float water = 0.f);
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
		float Mass;
		// Physical model
		shared_ptr<Seed> seed;
		vector<shared_ptr<Leaf>> Leaves;
		vector<shared_ptr<Stem>> Stems;
		vector<shared_ptr<Root>> Roots;
		// Graphical model
		shared_ptr<geometry::CMF> model;
	};
}
