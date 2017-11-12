#pragma once
#include "City.h"
#include "Culture.h"
#include "Occupation.h"
#include "Account.h"
#include "FederalBank.h"
#include "State.h"
#include "History.h"

using namespace History;
/* FEDERAL
The highest level of division; represents a single empire to govern all the state governments
*/
class Federal {
public:
	Federal() {};
	Federal(vector<City> cities);
	void Update();
	string GetName();
	int GetPop();
private:
	string m_name;
	vector<State> m_states;
	vector<Event> m_events; // a timeline of events
	FederalBank m_bank;
	IDhandler m_IDhandler;

	Inventory m_inventory;
	vector<Culture>* m_cultures;
	vector<Occupation>* m_occupations;
	Date m_date;
};

