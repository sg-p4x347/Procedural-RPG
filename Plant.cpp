#include "pch.h"
#include "Plant.h"

namespace world {
	Plant::Plant() :
		DnaCursor(0)
	{
	}
	void Plant::Leaf::Update(ExternalResource & external)
	{
		// Operational cost
		ParentPlant.Sugar -= 0.2f;

		float effectiveness = ParentPlant.Effectiveness(external.Temperature);
		float resourceConsumption = std::min(ParentPlant.Water, external.CO2);
		float produce = external.Light * resourceConsumption;
		// produce is capped by capacity
		produce = std::min(ParentPlant.AvailableCapacity(), produce);
		// Make the transaction
		external.CO2 -= resourceConsumption;
		ParentPlant.Water -= resourceConsumption;
		ParentPlant.Sugar += produce;

	}
	void Plant::Stem::Update(ExternalResource & external)
	{
		// Operational cost
		ParentPlant.Sugar -= 0.1f;
	}
	void Plant::Root::Update(ExternalResource & external)
	{
		// Operational cost
		ParentPlant.Sugar -= 0.2f;

		float effectiveness = ParentPlant.Effectiveness(external.Temperature);
		float transferAmount = std::min(ParentPlant.AvailableCapacity(), effectiveness * external.Water);
		// Make the transaction
		external.Water -= transferAmount;
		ParentPlant.Water += transferAmount;
	}
	void Plant::Seed::Update(ExternalResource & external)
	{
		// Operational cost
		ParentPlant.Sugar -= 0.1f;
	}
	Plant::PlantComponent::PlantComponent(Plant & plant) : ParentPlant(plant)
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
}
