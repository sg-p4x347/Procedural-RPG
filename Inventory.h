#pragma once
#include "Component.h"
namespace Components {
	struct InventoryItem {
		InventoryItem() {}
		InventoryItem(unsigned int typeEntity, bool procedural, unsigned int quantity) :
			TypeEntity(typeEntity),
			Procedural(procedural),
			Quantity(quantity) {}
		// ID of entity that conains meta-data about this type
		unsigned int TypeEntity;
		// If true, the TypeEntity is located in the world domain
		bool Procedural;
		// Integral count of this item
		unsigned int Quantity;
	};

	class Inventory :
		public Component
	{
	public:
		Inventory();
		Inventory(float maxVolume, float maxWeight);
		// Data
		vector<InventoryItem> Items;
		float MaxVolume;
		float MaxWeight;
		bool Open;
		// Inherited via Component
		virtual string GetName() override;
		void Import(std::ifstream & ifs) override;
		void Export(std::ofstream & ofs) override;
	};
}

