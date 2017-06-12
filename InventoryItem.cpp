#include "pch.h"
#include "InventoryItem.h"


InventoryItem::InventoryItem()
{
	demand = 0.f;
	supply = 0.f;
	quantity = 0.f;
}

InventoryItem::InventoryItem(float d, float s, float q) {
	demand = d;
	supply = s;
	quantity = q;
}
void InventoryItem::Update(const int & scalar) {
	quantity += supply * scalar;
	quantity -= demand * scalar;
}
void InventoryItem::operator+=(InventoryItem & other) {
	demand += other.demand;
	supply += other.supply;
	quantity += other.quantity;
}
void InventoryItem::SetRates(InventoryItem & other) {
	demand = other.demand;
	supply = other.supply;
}
