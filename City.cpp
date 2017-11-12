#include "pch.h"
#include "City.h"

namespace Components {
	City::City(const unsigned int & id) : Component::Component(id)
	{
	}

	City::City(const unsigned int & id,string name, DirectX::SimpleMath::Rectangle area) : Component::Component(id), m_name(name), m_area(area)
	{
	}

	City::~City() {
	}

	string City::GetCityName()
	{
		return m_name;
	}

	DirectX::SimpleMath::Rectangle City::GetArea()
	{
		return m_area;
	}

	vector<unsigned int> City::GetBuildings()
	{
		return m_buildings;
	}

	

	void City::Import(std::ifstream & ifs)
	{
		JsonParser city(ifs);
		ID = city["id"].To<unsigned int>();
		m_name = city["name"].To<string>();
		for (JsonParser & building : city["buildings"].GetElements()) {
			m_buildings.push_back(building.To<unsigned int>());
		}
	}
	void City::Export(std::ofstream & ofs)
	{
		JsonParser city;
		city.Set("id", ID);
		city.Set("name", m_name);
		JsonParser buildings(JsonType::array);
		for (unsigned int & building : m_buildings) {
			buildings.Add(building);
		}
		city.Set("buildings", buildings);
		city.Export(ofs);
	}
	string City::GetName()
	{
		return "City";
	}
}