#pragma once
#include "City.h"
#include "Culture.h"
#include "Occupation.h"
#include "Local.h"
#include "Account.h"

/* STATE
The third level of division; represents large regions that contain several cities
*/
class State {
public:
	State(std::vector<City> cities, std::vector<Culture> * cultures, vector<Occupation> * occupations, Account * account);
	void Update(Date date);
	void Buy(State & seller);

	vector<Local> Locals;
	bool selling;
	bool buying;
	int GDP;

	string GetName();
	int GetPop();
private:
	string m_name;

	vector<Event> m_events; // a timeline of events
	PrivateBank m_bank;
	int ID;
	IDhandler m_IDhandler;
};