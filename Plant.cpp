#include "pch.h"
#include "Plant.h"

namespace world {
	Plant::PlantComponent::PlantComponent(PlantComponent * parent, Matrix transform) :
		ParentPlant(parent->ParentPlant),
		Parent(parent),
		Transform(transform)
	{
		WorldMatrix = (Parent ? Parent->WorldMatrix : Matrix::Identity) * transform;
	}
	Plant::PlantComponent::PlantComponent(Plant & plant,Matrix transform) : ParentPlant(plant), Transform(transform), Parent(nullptr)
	{
		WorldMatrix = (Parent ? Parent->WorldMatrix : Matrix::Identity) * transform;
	}
	void Plant::PlantComponent::NthComponents(int n,vector<PlantComponent*> & components)
	{
		if (n == 0) {
			components.push_back(this);
		}
		else {
			for (auto & child : Children) {
				child->NthComponents(n - 1, components);
			}
		}
	}
	Plant::Plant(float sugar, float water) : 
		DnaCursor(0),
		OptimalTemperature(20.f),
		TemperatureTolerance(15.f),
		Sugar(sugar), 
		Water(water),
		Mass(0.f),
		model(new geometry::CMF("plant"))
	{
		shared_ptr<geometry::Material> material = std::make_shared<geometry::Material>("wood");
		material->textures.push_back("wood.dds");
		material->pixelShader = "Wood.cso";
		model->AddMaterial(material);
		model->AddLOD();
		model->AddMesh(std::make_shared<geometry::Mesh>());
	}
	float Plant::AvailableCapacity()
	{
		return Mass - (Sugar + Water);
	}
	float Plant::Effectiveness(float temperature)
	{
		return Utility::Gaussian(temperature, 1.f, OptimalTemperature, TemperatureTolerance);

	}
	void Plant::CreateSugar(ExternalResource & external)
	{
		// Operational cost
		Sugar -= 0.2f;

		float effectiveness = Effectiveness(external.Temperature);
		float resourceConsumption = std::min(Water, external.CO2);
		float produce = external.Light * resourceConsumption;
		// produce is capped by capacity
		produce = std::min(AvailableCapacity() * 0.5f, produce);
		// Make the transaction
		external.CO2 -= resourceConsumption;
		Water -= resourceConsumption;
		Sugar += produce;
	}
	void Plant::CollectWater(ExternalResource & external)
	{
		// Operational cost
		Sugar -= 0.2f;

		float effectiveness = Effectiveness(external.Temperature);
		float transferAmount = std::min(AvailableCapacity() * 0.5f, effectiveness * external.Water);
		// Make the transaction
		external.Water -= transferAmount;
		Water += transferAmount;
	}
	Plant::Leaf::Leaf(PlantComponent * parent, Matrix transform) : PlantComponent::PlantComponent(parent,transform)
	{
		ParentPlant.Sugar -= 0.1f;
	}
	vector<Vector3> Plant::Leaf::GetVertices()
	{
		vector<Vector3> vertices;
		vertices.push_back(Vector3::Transform(-Vector3::UnitZ * 0.5f, WorldMatrix));
		vertices.push_back(Vector3::Transform(Vector3::UnitZ * 0.5f, WorldMatrix));
		vertices.push_back(Vector3::Transform(Vector3::UnitX * 1.f, WorldMatrix));
		return vertices;
	}
	void Plant::Leaf::Grow(float radius)
	{
	}
	float Plant::Leaf::GetMass()
	{
		return 0.01f;
	}
	Plant::Stem::Stem(PlantComponent * parent, Matrix transform, float length, float radius) :
		PlantComponent::PlantComponent(parent, transform),
		Length(length),
		Radius(0.f)
	{
		Grow(radius);
	}
	Vector3 Plant::Stem::GetStart()
	{
		return Vector3::Transform(Vector3::Zero, WorldMatrix);
	}
	Vector3 Plant::Stem::GetEnd()
	{
		return Vector3::Transform(Vector3::UnitY, WorldMatrix);
	}
	void Plant::Stem::Grow(float radius)
	{
		float initialMass = GetMass();
		Radius += radius;
		float finalMass = GetMass();
		float deltaMass = finalMass - initialMass;
		ParentPlant.Mass += deltaMass;
		ParentPlant.Sugar -= deltaMass;
	}
	float Plant::Stem::GetMass()
	{
		// Cylinder
		return XM_PI * Radius * Radius * Length;
	}
	Plant::Root::Root(PlantComponent * parent, Matrix transform, float radius) : PlantComponent::PlantComponent(parent, transform), Radius(0.f)
	{
		Grow(radius);
	}
	Vector3 Plant::Root::GetCenter()
	{
		return Vector3::Transform(Vector3::Zero,WorldMatrix);
	}
	Vector3 Plant::Root::GetNormal()
	{
		return Vector3::Transform(Vector3::UnitY, WorldMatrix);
	}
	void Plant::Root::Grow(float radius)
	{
		float initialMass = GetMass();
		Radius += radius;
		float finalMass = GetMass();
		float deltaMass = finalMass - initialMass;
		ParentPlant.Mass += deltaMass;
		ParentPlant.Sugar -= deltaMass;
	}
	float Plant::Root::GetMass()
	{
		// Half Sphere
		return (2.f / 3.f ) * XM_PI * Radius * Radius * Radius;
	}
	Plant::Seed::Seed(Plant & plant, Matrix transform, float radius) : Plant::PlantComponent::PlantComponent(plant,transform), Radius(radius)
	{
	}
	void Plant::Seed::Grow(float radius)
	{
	}
	float Plant::Seed::GetMass()
	{
		// Sphere
		return (4.f / 3.f) * XM_PI * Radius * Radius * Radius;
	}
	Plant::GrowNewComponent::GrowNewComponent() :
		componentIndex(0),
		position(0.f),
		type(StemComponent),
		yawPitchRoll()
	{
		
	}
}
