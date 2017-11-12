#pragma once
#include "Account.h"
#include "History.h"
#include "Occupation.h"

using namespace std;
using namespace History;

class Citizen {
public:
	Citizen(int ID, Account * a);
	void Update(Date date);
	// returns the income of the seller
	float Buy(Citizen & seller);
	void SetCulture(string & culture);
	string GetCulture();
	void SetOccupation(Occupation & occ);
	Occupation GetOccupation();
	void SetInventory(Inventory & inv);
	Inventory GetInventory();
	Account* GetAccount();
private:
	string m_culture;
	OccupationType m_occupation;
	Inventory m_inventory;
	Account * m_account;
	int m_ID;
};

