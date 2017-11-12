#include "pch.h"
#include "Inventory.h"


Inventory::Inventory()
{
	currency = 0;
	selling = false;
	buying = false;
	// necessity
	items.insert(pair<string, InventoryItem>("food", InventoryItem()));
	items.insert(pair<string, InventoryItem>("water", InventoryItem()));
	// services
	items.insert(pair<string, InventoryItem>("safety", InventoryItem()));
	// resources
	items.insert(pair<string, InventoryItem>("wood", InventoryItem()));
	items.insert(pair<string, InventoryItem>("stone", InventoryItem()));
	items.insert(pair<string, InventoryItem>("iron", InventoryItem()));
	items.insert(pair<string, InventoryItem>("coal", InventoryItem()));
	items.insert(pair<string, InventoryItem>("gold", InventoryItem()));
	items.insert(pair<string, InventoryItem>("silver", InventoryItem()));
	// manufactured
	items.insert(pair<string, InventoryItem>("tools", InventoryItem()));
	items.insert(pair<string, InventoryItem>("shelter", InventoryItem()));
}
void Inventory::Update(int pop)
{
	selling = false;
	buying = false;
	// update supply and demand
	for (map<string, InventoryItem>::iterator i = items.begin(); i != items.end(); i++) {
		i->second.Update(pop);
		if (i->second.quantity < 0) {
			buying = true;
		}
		else if (i->second.quantity > 0) {
			selling = true;
		}
	}
}
void Inventory::operator+=(Inventory & other)
{
	currency += other.currency;

	for (map<string, InventoryItem>::iterator i = items.begin(); i != items.end(); i++) {
		i->second += other.items[i->first];
	}
}
// attempts to first: buy with money, second: trade, third: go into debt
// returns the quantity of debt that was taken on
float Inventory::Buy(Inventory & seller, float & credit)
{
	int income = 0;
	bool notSelling = true;
	bool notBuying = true;
	for (map<string, InventoryItem>::iterator i = items.begin(); i != items.end(); i++) {
		string itemName = i->first;

		if (items[itemName].quantity < 0 && seller.items[itemName].quantity > 0) {
			// set the transaction based off of resource counts
			float transactionQuantity = min(-items[itemName].quantity, seller.items[itemName].quantity);
			// adjust for the maximum ammount of allowed credit
			transactionQuantity = min(transactionQuantity, currency + credit);
			// make the transaction
			credit -= max(transactionQuantity - currency, 0.f);
			currency -= min(transactionQuantity, currency);

			items[itemName].quantity += transactionQuantity;
			seller.currency += transactionQuantity;
			seller.items[itemName].quantity -= transactionQuantity;

			income += transactionQuantity;
		}
		// selling and buying will only be false if neither of these conditions is ever true
		if (seller.items[itemName].quantity > 0) {
			notSelling = false;
		}
		if (items[itemName].quantity < 0) {
			notBuying = false;
		}
	}
	// set selling and buying flags (for increased performance)
	seller.selling = !notSelling;
	buying = !notBuying;

	return income;
}

void Inventory::SetRates(Inventory & other)
{
	for (map<string, InventoryItem>::iterator i = items.begin(); i != items.end(); i++) {
		i->second.SetRates(other.items[i->first]);
	}
}

bool Inventory::IsProfitable(Inventory & other)
{
	for (map<string, InventoryItem>::iterator i = items.begin(); i != items.end(); i++) {
		if (i->second.supply > 0 && other.items[i->first].demand > other.items[i->first].supply) {
			return true;
		}
	}
	return false;
}

