#include "pch.h"
#include "State.h"

State::State(vector<City> cities, vector<Culture> * cultures, vector<Occupation> * occupations, Account * account) : m_bank(account)
{
	// construct local governances
	for (City & city : cities) {
		Locals.push_back(Local(city, cultures, occupations, m_bank.AddAccount()));
	}
}
void State::Update(Date date)
{
	// 0.) update the bank
	m_bank.Update();
	// 1.) update supply and demand
	for (Local & local : Locals) {
		local.Update(date);
	}
	// 2.) city trading (and debt creation)
	for (int buyer = 0; buyer < Locals.size(); buyer++) {
		if (Locals[buyer].buying) {
			// find a seller
			for (int seller = 0; seller < Locals.size(); seller++) {
				if (seller != buyer && Locals[seller].selling) {
					Locals[buyer].Buy(Locals[seller]);
				}
			}
		}
	}

}
void State::Buy(State & seller)
{

}
string State::GetName()
{
	return string();
}
int State::GetPop()
{
	return 0;
}