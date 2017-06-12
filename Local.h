#pragma once
#include "City.h"
#include "Culture.h"
#include "Occupation.h"
#include "Account.h"
#include "PrivateBank.h"
#include "Citizen.h"
#include "History.h"
using namespace History;
/* LOCAL
The second level of division; represents towns and cities and their governance over their citizens
*/
class Local {
public:
	Local(City city, vector<Culture> * cultures, vector<Occupation> * occupations, Account * account);
	void Update(Date date);
	void Buy(Local & seller);
	int GetPop();
	vector<Occupation> ProfitableOccupations();
	// give the slackers a paying job
	void Rebalance(vector<Occupation> & profitable);
	vector<Citizen> Citizens;
	bool selling;
	bool buying;
	int GDP;

	Inventory & GetInventory();
	City & GetCity();
private:
	City m_city;
	vector<Occupation> * m_occupations;
	vector<Culture> * m_cultures;
	//vector<Culture> m_cultures;
	Inventory m_inventory;
	vector<Event> m_events; // a timeline of events
	PrivateBank m_bank;

	int m_ID;
	IDhandler m_IDhandler;

};

