#include "pch.h"
#include "City.h"
#include "Building.h"
#include "NameGenerator.h"
#include "JsonParser.h"

using namespace Architecture;
using namespace std;
using namespace DirectX::SimpleMath;
City::City()
{
}

City::City(JsonParser & city)
{
	Import(city);
}

City::City(SimpleMath::Rectangle area,const HeightMap & terrain, const HeightMap & biome) {
	JsonParser continentCfg(std::ifstream("config/continent.json"));
	m_area = area;
	m_biomes = map<string, float>();
	// generate name
	NameGenerator NG;
	m_name = NG.GetCityName();
	// total biome quantities
	vector<string> biomeNames = continentCfg["biomes"].GetKeys();
	int sampleSpacing = continentCfg["sampleSpacing"].To<int>();
	
	// loop over all the enclosed biome samples (spaced apart on the terrain by sampleSpacing)
	int sampleCount = 0;
	try {
		for (int biomeX = ceil(m_area.x / (float)sampleSpacing); biomeX <= floor((m_area.x + m_area.width) / (float)sampleSpacing); biomeX++) {
			for (int biomeY = ceil(m_area.y / (float)sampleSpacing); biomeY <= floor((m_area.y + m_area.height) / (float)sampleSpacing); biomeY++) {
				// add this biome sample to its respective mapping count
				m_biomes[GetBiomeName(biome.map[biomeX][biomeY])]++;
				sampleCount++;
			}
		}
	} catch (exception & ex) {
		int test;
	}
	// normalize biome mappings
	for (std::map<string,float>::iterator i = m_biomes.begin(); i != m_biomes.end(); i++) {
		i->second = i->second / (float)sampleCount;
	}

	// TEMP
	m_buildings.push_back(shared_ptr<Building>(new Building(Architecture::Rectangle(0l, 0l, 10l, 15l), JsonParser(std::ifstream("config/building.json")),"residential")));
}

City::~City() {
}

Vector2 City::GetPosition()
{
	return m_area.Center();
}

SimpleMath::Rectangle City::GetArea()
{
	return m_area;
}

vector<shared_ptr<Building>> City::GetBuildings()
{
	return m_buildings;
}

void City::SaveCity(string directory)
{
	Export().Export(std::ofstream(directory + '/' + m_name + ".json"));
}

string City::GetBiomeName(float value)
{
	JsonParser continentCfg("config/continent.json");
	vector<string> biomeNames = continentCfg["biomes"].GetKeys();
	for (string biomeName : biomeNames) {
		if (abs(continentCfg["biomes"][biomeName]["center"].To<double>() - value) <= continentCfg["biomes"][biomeName]["range"].To<double>()) {
			return biomeName;
		}
	}
	return "";
}

void City::Import(JsonParser & jp)
{
	m_name = jp["name"].To<string>();
	for (JsonParser & building : jp["buildings"].GetElements()) {
		m_buildings.push_back(shared_ptr<Building>(new Building(building)));
	}
}

JsonParser City::Export()
{
	JsonParser city;
	city.Set("name", m_name);
	JsonParser buildings(JsonType::array);
	for (shared_ptr<Building> & building : m_buildings) {
		buildings.Add(building->Export());
	}
	city.Set("buildings", buildings);
	return city;
}
