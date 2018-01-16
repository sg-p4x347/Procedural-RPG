#include "pch.h"
#include "Citizen.h"

Citizen::Citizen(int id, Account * a) : m_account(a)
{
	m_ID = id;
	m_account->income = 10;
}

void Citizen::Update(Date date)
{
	// attempt to pay off debt
	m_account->Compensate(m_inventory.currency);
	// produce this year's resources
	m_inventory.Update(1);
}

float Citizen::Buy(Citizen & seller)
{
	float income = m_inventory.Buy(seller.GetInventory(), m_account->credit);
	seller.GetAccount()->income = income;
	return income;
}
void Citizen::SetCulture(string & culture)
{
	m_culture = culture;
}
string Citizen::GetCulture()
{
	return string();
}
void Citizen::SetOccupation(Occupation & occ)
{
	m_occupation = occ.type;
	m_inventory.SetRates(occ.inventory);

}
Occupation Citizen::GetOccupation()
{
	return m_occupation;
}
void Citizen::SetInventory(Inventory & inv)
{
}
Inventory Citizen::GetInventory()
{
	return Inventory();
}
Account * Citizen::GetAccount()
{
	return m_account;
}
