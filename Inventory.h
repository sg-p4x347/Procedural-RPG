#pragma once
#include "ISerialization.h"
#include "WorldComponent.h"
namespace world {
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
		public WorldComponent,
		public ISerialization
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
		void Import(std::istream & ifs) override;
		void Export(std::ostream & ofs) override;
	};
}

