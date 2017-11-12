#include "pch.h"
#include "InfrastructureSystem.h"
#include "NameGenerator.h"
#include "ProUtil.h"
#include "City.h"
#include "Position.h"
InfrastructureSystem::InfrastructureSystem(shared_ptr<EntityManager> & entityManager, vector<string> & components, unsigned short updatePeriod) : System::System(entityManager,components,updatePeriod)
{
}


InfrastructureSystem::~InfrastructureSystem()
{
}

string InfrastructureSystem::Name()
{
	return "Infrastructure";
}

void InfrastructureSystem::Update(double & elapsed)
{
}

void InfrastructureSystem::SyncEntities()
{
	System::SyncEntities();
	for (auto & entity : m_entities) {
		if (entity->HasComponents(EM->ComponentMask("City"))) {
			m_cities.push_back(entity);
		}
	}
}

void InfrastructureSystem::Generate()
{
	JsonParser cityCfg(std::ifstream("config/city.json"));
	const int minCities = cityCfg["MinCities"].To<int>();
	const int maxCities = cityCfg["MaxCities"].To<int>();
	const int cityCount = ProUtil::RandWithin(minCities, maxCities);
	for (int i = 0; i < cityCount; i++) {
		CreateCity();
	}
}

void InfrastructureSystem::SetTerrainSystem(shared_ptr<TerrainSystem>& terrainSystem)
{
	TS = terrainSystem;
}

void InfrastructureSystem::CreateCity()
{
	JsonParser continentCfg(std::ifstream("config/continent.json"));
	JsonParser cityCfg(std::ifstream("config/city.json"));
	const float minElevation = cityCfg["MinElevation"].To<float>();
	const float maxElevation = cityCfg["MaxElevation"].To<float>();
	// determine a rectangular footprint
	DirectX::SimpleMath::Rectangle area = ProUtil::RectWithin(ProUtil::GoldenRect(cityCfg["MinArea"].To<double>()), ProUtil::GoldenRect(cityCfg["MaxArea"].To<double>()));
	// find a suitable patch of land
	
	for (int tries = 0; tries < 100; tries++) {
		DirectX::SimpleMath::Rectangle validArea = TS->Area();
		validArea.Inflate(-area.width, -area.height);
		Vector2 center = ProUtil::PointWithin(validArea);
		area.x = center.x - area.Center().x;
		area.y = center.y - area.Center().y;
		float height = TS->Height(area.Center().x, area.Center().y);
		if (height >= minElevation && height <= maxElevation) {
			// make a new entity
			shared_ptr<Entity> city = EM->NewEntity();
			// generate name
			NameGenerator NG;
			city->AddComponent(EM->ComponentMask("City"),
				shared_ptr<Components::Component>(new Components::City(city->ID(),NG.GetCityName(),area)));
			city->AddComponent(EM->ComponentMask("Position"),
				shared_ptr<Components::Component>(new Components::Position(city->ID(), Vector3(area.Center().x,height,area.Center().y),Vector3::Zero)));
			return;
		}
	}
	//JsonParser(std::ifstream("config/building.json")), "residential")));
}
