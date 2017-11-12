#pragma once
#include <map>
#include "InventoryItem.h"

using namespace std;

struct Inventory {
	Inventory();
	// updates item quantities based off of supply and demand
	void Update(int pop);
	// adds an inventory to this
	void operator+=(Inventory & other);
	// takes a reference to the seller's inventory, a ref to buyer's bank account credit
	// and returns the transaction quantity (used to set the seller's income)
	float Buy(Inventory & seller, float & credit);
	// copies supply and demand rates from other inventory
	void SetRates(Inventory & other);
	bool IsProfitable(Inventory & other);

	//===========================
	// properties
	//===========================
	float currency;
	map<string, InventoryItem> items;
	bool selling = false;
	bool buying = false;
};

