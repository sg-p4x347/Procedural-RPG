#pragma once
struct InventoryItem {
	InventoryItem();
	InventoryItem(float d, float s, float q);
	void Update(const int & scalar);
	void operator+=(InventoryItem & other);
	void SetRates(InventoryItem & other);
	// Properties
	float demand;   // how much the value is needed (0.0 <-> 1.0, A percentage of how much of the parent needs it)
	float supply;	  // how much of the value can be produced (0.0 <-> 1.0, A percentage of how much of the parent can produce it)
	float quantity; // how much of the value is owned (0.0 <-> 1.0, A percentage of how much of the parent owns it)
};

