#include "pch.h"
#include "Local.h"


//------------------------------
// LOCAL
//------------------------------
Local::Local(City city, vector<Culture> * cultures, vector<Occupation> * occupations, Account * account) : m_bank(account)
{
	m_city = city;
	m_occupations = occupations;
	m_cultures = cultures;
	m_IDhandler = IDhandler();
	// create the initial population
	for (int i = 0; i < 30; i++) {
		Citizen citizen = Citizen(m_IDhandler.GetID(), m_bank.AddAccount());
		citizen.SetCulture((*m_cultures)[randWithin(0, m_cultures->size())].name);
		citizen.SetOccupation((*m_occupations)[randWithin(0, m_occupations->size())]);

		Citizens.push_back(citizen);
	}
}
void Local::Update(Date date)
{
	// 3.) update the bank
	m_bank.Update();
	// 0.) rebalance jobs every once in a while
	if (date.year % 5 == 2) {
		Rebalance(ProfitableOccupations());
	}
	// 1.) update supply and demand (and debt compensation)
	for (Citizen & citizen : Citizens) {
		citizen.Update(date);
	}
	// 2.) citizen trading (and debt creation)
	for (int buyer = 0; buyer < Citizens.size(); buyer++) {
		if (Citizens[buyer].GetInventory().buying) {
			// find a seller
			// start with a random seller (so they don't always choose the same one)
			int offset = randWithin(0, Citizens.size());
			for (int seller = 0; seller < Citizens.size(); seller++) {
				if (!Citizens[buyer].GetInventory().buying) break;
				if (seller != buyer && Citizens[(seller + offset) % Citizens.size()].GetInventory().selling) {
					Citizens[buyer].Buy(Citizens[(seller + offset) % Citizens.size()]);
				}
			}
		}
	}

}
void Local::Buy(Local & seller)
{
	bool notSelling = true;
	bool notBuying = true;
	// trade between citizens in separate cities (with import/export tax of course)
	for (int buyerIndex = 0; buyerIndex < Citizens.size(); buyerIndex++) {
		if (Citizens[buyerIndex].GetInventory().buying) {
			// find a seller

			for (int sellerIndex = 0; sellerIndex < seller.Citizens.size(); sellerIndex++) {
				if (sellerIndex != buyerIndex && Citizens[sellerIndex].GetInventory().selling) {
					// make the transaction
					Citizens[buyerIndex].Buy(Citizens[sellerIndex]);
					// selling and buying will only be false if neither of these conditions is ever true
					if (seller.Citizens[sellerIndex].GetInventory().selling) {
						notSelling = false;
					}
					if (Citizens[buyerIndex].GetInventory().buying) {
						notBuying = false;
					}
				}
			}
		}

	}
	// set selling and buying flags (for increased performance)
	seller.selling = !notSelling;
	buying = !notBuying;
}
int Local::GetPop()
{
	return Citizens.size();
}

vector<Occupation> Local::ProfitableOccupations()
{
	vector<Occupation> profitableOccupations;
	Inventory summary = Inventory();
	for (Citizen citizen : Citizens) {
		summary += citizen.GetInventory();
	}
	for (Occupation occ : *m_occupations) {
		if (occ.inventory.IsProfitable(summary)) {
			profitableOccupations.push_back(occ);
		}
	}
	return profitableOccupations;
}
void Local::Rebalance(vector<Occupation>& profitable)
{
	if (profitable.size() > 0) {
		// if the citizen has no money, they get a new job
		for (Citizen citizen : Citizens) {
			if (citizen.GetInventory().currency <= 0) {
				citizen.SetOccupation(profitable[randWithin(0, profitable.size())]);
			}
			else if (citizen.GetInventory().items["food"].quantity < 0) {
				// Food is important, so if there is a shortage... become a farmer!
				for (Occupation occ : *m_occupations) {
					if (occ.inventory.items["food"].supply > 0) {
						citizen.SetOccupation(occ);
						break;
					}
				}
			}
		}
	}
}

Inventory & Local::GetInventory()
{
	return m_inventory;
}
City & Local::GetCity()
{
	return m_city;
}
