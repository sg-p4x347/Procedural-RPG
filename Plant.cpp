#include "pch.h"
#include "Plant.h"

namespace world {
	Plant::Plant() :
		DnaCursor(0)
	{
	}
	Plant::PlantComponent::PlantComponent(Plant & plant,Matrix & transform) : ParentPlant(plant), Transform(transform)
	{
	}
	void Plant::PlantComponent::NthComponents(int n,vector<shared_ptr<PlantComponent>> & components)
	{
		if (n == 1) {
			components.insert(components.end(), Children.begin(), Children.end());
		}
		else {
			for (auto & child : Children) {
				NthComponents(n - 1, components);
			}
		}
	}
	float Plant::AvailableCapacity()
	{
		return Volume - (Sugar + Water);
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
		produce = std::min(AvailableCapacity(), produce);
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
		float transferAmount = std::min(AvailableCapacity(), effectiveness * external.Water);
		// Make the transaction
		external.Water -= transferAmount;
		Water += transferAmount;
	}
	Plant::Leaf::Leaf(Plant & plant, Matrix & transform) : PlantComponent::PlantComponent(plant,transform)
	{

	}
	void Plant::Leaf::Grow(float radius)
	{
	}
	float Plant::Leaf::GetMass()
	{
		return 0.01f;
	}
	Plant::Stem::Stem(Plant & plant, Matrix & transform, float length, float radius) : PlantComponent::PlantComponent(plant, transform)
	{
	}
	void Plant::Stem::Grow(float radius)
	{
		float initialMass = GetMass();
		Radius += radius;
		float finalMass = GetMass();
		ParentPlant.Sugar -= (finalMass - initialMass);
	}
	float Plant::Stem::GetMass()
	{
		return XM_PI * Radius * Radius * Length;
	}
	void Plant::Root::Grow(float radius)
	{
		float initialMass = GetMass();
		Radius += radius;
		float finalMass = GetMass();
		ParentPlant.Sugar -= (finalMass - initialMass);
	}
	float Plant::Root::GetMass()
	{
		return XM_PI * Radius * Radius;
	}
}
