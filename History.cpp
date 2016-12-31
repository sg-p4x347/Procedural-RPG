#include "pch.h"
#include "History.h"

namespace History {
	//------------------------------
	// Social
	//------------------------------
	Social::Social(string name, int pop)
	{
		m_name = name;
		m_population = pop;
		// set initial supply
		m_values.food.supply = 1.1;
		m_values.water.supply = 1.1;
		m_values.wood.supply = 1.1;
		m_values.stone.supply = 1.1;
		// set initial demand
		m_values.food.demand = 1.0;
		m_values.water.demand = 1.0;
	}
	void Social::Update(Date date)
	{
		
		// produce resources
		m_values.food.quantity += m_values.food.supply;
		m_values.water.quantity += m_values.water.supply;
		m_values.wood.quantity += m_values.wood.supply;
		m_values.stone.quantity += m_values.stone.supply;
		m_values.shelter.quantity += m_values.shelter.supply;

		// Consume resources
		m_values.food.quantity -= m_values.food.demand;
		m_values.water.quantity -= m_values.water.demand;
		// Import resources
		m_values.wealth.demand = 0;
		// total up the demand for imported resources
		m_values.wealth.demand -= min(m_values.food.quantity,0.f);
		m_values.wealth.demand -= min(m_values.food.quantity,0.f);
		// proportionately import based on demand magnitude
		if (m_values.food.quantity < 0) {
			m_values.food.quantity += max((-m_values.food.quantity / m_values.wealth.demand)*m_values.wealth.quantity, -m_values.food.quantity);
		}
		if (m_values.water.quantity < 0) {
			m_values.water.quantity += max((-m_values.water.quantity / m_values.wealth.demand)*m_values.wealth.quantity, -m_values.water.quantity);
		}
		// set a demand for shelter
		m_values.shelter.demand = min({ m_values.food.quantity,m_values.water.quantity });
		// use domestic products to meet the demand for shelter
		m_values.wood.demand = min(min(m_values.shelter.demand / 2, m_values.wood.quantity),min(m_values.shelter.demand / 2, m_values.stone.quantity));
		m_values.stone.demand = m_values.wood.demand;
		m_values.shelter.quantity += m_values.wood.demand + m_values.stone.demand;
		m_values.wood.quantity -= m_values.wood.demand;
		m_values.wood.demand = 0;
		m_values.stone.quantity -= m_values.stone.demand;
		m_values.stone.demand = 0;
		// Export
		m_values.wealth.quantity += m_values.wood.quantity;
		m_values.wood.quantity = 0;
		m_values.wealth.quantity += m_values.stone.quantity;
		// population growth model
		float deltaCapacity = min({ m_values.food.quantity,m_values.water.quantity,m_values.shelter.quantity });
		if (deltaCapacity < 0) {
			m_events.push_back(Event("Famine", date, date));
		}
		m_population += m_population*deltaCapacity;
	}
	string Social::GetName()
	{
		return m_name;
	}
	int Social::GetPop()
	{
		return m_population;
	}
	Values Social::GetValues()
	{
		return m_values;
	}
	int Social::Logistic(int time)
	{
		return 1 / (1 - pow(2.7, -time));
	}
	//------------------------------
	// LOCAL
	//------------------------------
	Local::Local(string name)
	{
		m_name = name;
	}
	void Local::Update()
	{
	}
	string Local::GetName()
	{
		return m_name;
	}
	int Local::GetPop()
	{
		int population = 0;
		for (Social social : m_socials) {
			population += social.GetPop();
		}
		return population;
	}
	Values Local::GetValues()
	{
		return m_values;
	}
	Federal::Federal()
	{
	}
	Federal::Federal(string name)
	{
		m_name = name;
	}
	string Federal::GetName()
	{
		return string();
	}
	Timeline::Timeline()
	{
		m_date.year = 0;
		m_date.season = Spring;
	}
}
