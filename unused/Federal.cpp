#include "pch.h"
#include "Federal.h"
#include "JsonParser.h"
#include <map>
Federal::Federal(vector<City> cities)
{
	JsonParser cfg(std::ifstream("config/history.json"));
	NameGenerator ng = NameGenerator();
	m_name = ng.GetFederalName();
	// Generate the various culture groups
	int maxCultures = randWithin(cfg["cultures"]["min"].To<int>(), cfg["cultures"]["max"].To<int>());
	m_cultures = new vector<Culture>();
	for (int i = 0; i < maxCultures; i++) {
		m_cultures->push_back(Culture());
	}
	// Create the occupations
	m_occupations = new vector<Occupation>{
		Occupation(Logger),
		Occupation(Miner),
		Occupation(Blacksmith),
		Occupation(Soldier),
		Occupation(Farmer),
		Occupation(Carpenter)
	};
	// initialize the bank
	m_bank = FederalBank();
	// generate the states by grouping cities based on proximity
	int maxStates = randWithin(cfg["states"]["min"].To<int>(), cfg["states"]["max"].To<int>());
	int citiesPer = floor(cities.size() / maxStates);
	for (int i = 0; i < maxStates; i++) {
		vector<City> stateCities;
		for (int k = 0; k < (i == maxStates - 1 ? cities.size() : citiesPer); k++) {
			int closest = 0;
			double distance;
			// add the first (seed) city right away, then check distances for the rest
			if (stateCities.size() > 0) {
				for (City & member : stateCities) {
					for (int j = 1; j < cities.size(); j++) {
						if (j == 1 || pythag(cities[j].GetPosition().x - member.GetPosition().x, cities[j].GetPosition().y - member.GetPosition().y, 0.0) < distance) {
							distance = pythag(cities[j].GetPosition().x - member.GetPosition().x, cities[j].GetPosition().y - member.GetPosition().y, 0.0);
							closest = j;
						}
					}
				}
			}
			stateCities.push_back(cities[closest]);
			cities.erase(cities.begin() + closest);
		}
		m_states.push_back(State(stateCities, m_cultures, m_occupations, m_bank.AddAccount()));
	}

}
void Federal::Update()
{
	JsonParser historyCfg(std::ifstream("config/history.json"));
	int maxIteration = randWithin(historyCfg["iterations"]["min"].To<int>(), historyCfg["iterations"]["max"].To<int>());
	// start the iterative process of simulating each year
	for (int year = 0; year < maxIteration; year++) {
		m_bank.Update();
		for (State & state : m_states) {
			state.Update(m_date);
		}

		m_date.year++;
	}

}
string Federal::GetName()
{
	return string();
}
int Federal::GetPop()
{
	return 0;
}
