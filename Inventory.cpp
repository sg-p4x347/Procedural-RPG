#include "pch.h"
#include "Inventory.h"
namespace world {

	Inventory::Inventory() : Inventory::Inventory(0.f, 0.f)
	{
	}

	Inventory::Inventory(float maxVolume, float maxWeight) : MaxVolume(maxVolume), MaxWeight(maxWeight), Open(false)
	{
	}

	void Inventory::Import(std::istream & ifs)
	{
		int itemCount = 0;
		DeSerialize(itemCount, ifs);
		for (int i = 0; i < itemCount; i++) {
			InventoryItem item;
			DeSerialize(item.TypeEntity, ifs);
			DeSerialize(item.Procedural, ifs);
			DeSerialize(item.Quantity, ifs);
			Items.push_back(item);
		}
		DeSerialize(MaxVolume, ifs);
		DeSerialize(MaxWeight, ifs);
		DeSerialize(Open, ifs);
	}
	void Inventory::Export(std::ostream & ofs)
	{
		int itemCount = Items.size();
		Serialize(itemCount, ofs);
		for (auto & item : Items) {
			Serialize(item.TypeEntity, ofs);
			Serialize(item.Procedural, ofs);
			Serialize(item.Quantity, ofs);
		}
		Serialize(MaxVolume, ofs);
		Serialize(MaxWeight, ofs);
		Serialize(Open, ofs);
	}
}
