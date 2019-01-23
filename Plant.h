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
			PlantComponent(PlantComponent* parent, Matrix transform);
			PlantComponent(Plant & plant, Matrix transform);
			vector<shared_ptr<PlantComponent>> Children;
			PlantComponent* Parent;
			Plant & ParentPlant;
			Matrix Transform;
			Matrix WorldMatrix;

			void NthComponents(int n, vector<PlantComponent*> & components);
			virtual void Grow(float radius) = 0;
			virtual float GetMass() = 0;
		};
		struct Leaf : public PlantComponent {
			Leaf(PlantComponent * parent, Matrix transform);
			
			vector<Vector3> GetVertices();
			void Grow(float radius) override;
			float GetMass() override;
		};
		struct Stem : public PlantComponent {
			Stem(PlantComponent * parent, Matrix transform, float length, float radius);
			float Length;
			float Radius;
			
			Vector3 GetStart();
			Vector3 GetEnd();
			void Grow(float radius) override;
			float GetMass() override;
		};
		struct Root : public PlantComponent {
			Root(PlantComponent * parent, Matrix transform, float radius);
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
		struct Action {
			Action() {}
			virtual ~Action() {}
		};
		struct Grow : public Action {
			int componentIndex;
			float radius;
		};
		struct GrowNewComponent : public Action {
			GrowNewComponent();
			int componentIndex;
			ComponentTypes type;
			float position;
			Vector3 yawPitchRoll;
		};
		struct CollectWaterAction : public Action {
			float amount;
		};
		struct CreateSugarAction : public Action {
			float amount;
		};
		
	public:
		Plant(float sugar = 0.f, float water = 0.f);
		float AvailableCapacity();
		float Effectiveness(float temperature);
		void CreateSugar(ExternalResource & external);
		void CollectWater(ExternalResource & external);
		template<typename ActionType>
		void AddAction(ActionType action) {
			std::shared_ptr<Action> base((Action *)new ActionType(action));
			DNA.push_back(base);
		}
	public:
		// Genetic Data
		vector<std::shared_ptr<Action>> DNA;
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
