#include "pch.h"
#include "RegionSystem.h"


RegionSystem::RegionSystem(
	shared_ptr<EntityManager> & entityManager,
	vector<string> & components,
	unsigned short updatePeriod,
	string directory,
	int worldWidth,
	int regionWidth) : System(entityManager,components,updatePeriod), m_directory(directory)
{


	// initialize the regions
	for (int x = 0; x < worldWidth / regionWidth; x++) {
		m_regions.push_back(vector<Region>());
		for (int z = 0;z < worldWidth / regionWidth; z++) {
			Region region(x, z, worldWidth, regionWidth);

			// search for the player
			m_entityManager->Load(region, m_entityManager->ComponentMask("Player"));
			if (m_entityManager->GetComponent())
			m_regions[x].push_back(region);
		}
	}
	// find the player

}


RegionSystem::~RegionSystem()
{
}

void RegionSystem::Update()
{
}
void RegionSystem::LoadRegions()
{
	m_entityManager->GetComponent<Position>()
	// calculate displacement
	int regionX = int(round(m_player->getPosition().x / float(m_regionWidth)));
	int regionZ = int(round(m_player->getPosition().z / float(m_regionWidth)));
	// get the region displacement
	int displacementX = regionX - m_lastX;
	int displacementZ = regionZ - m_lastZ;
	// update the position of last region
	m_lastX = regionX;
	m_lastZ = regionZ;
	if (displacementX == 0 && displacementZ == 0) {
		// don't load anything
		return;
	}
	else if (abs(displacementX) <= 1 && abs(displacementZ) <= 1) {
		// roll the new regions into the old one's places
		m_regions->offsetX(displacementX);
		m_regions->offsetY(displacementZ);
		if (abs(displacementX) == 1) {
			// the left or right most collumn is replaced with the new regions
			int x;
			if (displacementX == -1) {
				x = 0;
			}
			else if (displacementX == 1) {
				x = m_loadWidth - 1;
			}
			for (int z = 0; z < m_loadWidth; z++) {
				// Initialize the new region into old one's place
				Region* region = m_regions->get(x, z);
				region->Initialize(
					m_d3dDevice.Get(),
					regionX - int(round(m_loadWidth / 2.f)) + x,
					regionZ - int(round(m_loadWidth / 2.f)) + z,
					m_worldWidth, m_regionWidth, m_name,
					BuildingsInRegion(region->GetArea())
				);
			}
		}
		if (abs(displacementZ) == 1) {
			// the left or right most row is replaced with the new regions
			int z;
			if (displacementZ == -1) {
				z = 0;
			}
			else if (displacementZ == 1) {
				z = m_loadWidth - 1;
			}
			for (int x = 0; x < m_loadWidth; x++) {
				// place the new region into old one's place
				Region* region = m_regions->get(x, z);
				region->Initialize(
					m_d3dDevice.Get(),
					regionX - int(round(m_loadWidth / 2.f)) + x,
					regionZ - int(round(m_loadWidth / 2.f)) + z,
					m_worldWidth, m_regionWidth, m_name,
					BuildingsInRegion(region->GetArea())
				);
			}
		}
	}
	else {
		// load a whole new set of regions and reset the circular array
		FillRegions();
	}
}

void RegionSystem::SaveEntities(Region & region, unsigned long componentMask)
{
	// save entities that:
	// - are within the specified region
	// - and satisfy the componentMask

	//JsonParser entities(JsonType::array);

	//componentMask |= m_entityManager->ComponentMask("Position");
	//for (Entity & entity : m_entityManager->Entities(componentMask)) {
	//	Position position = *(dynamic_cast<Position*>(entity.GetComponents()["Position"].get()));
	//	if (region.GetArea().Contains(Vector2(position.Pos.x, position.Pos.z))) {
	//		// this entity is in this region
	//		// save all of its components
	//		JsonParser entityJSON = entity.Export();

	//		auto components = entity.GetComponents();
	//		for (auto & pair : components) {
	//			entityJSON["components"].Set(pair.first, pair.second->Export());
	//		}
	//	}
	//}

	//// save to a file (name is the component mask)
	//std::ofstream ofs(region.GetDirectory() + "/" + std::to_string(componentMask) + ".dat");
	//ofs << entities.ToString();
}

void RegionSystem::SyncEntities()
{
	m_entities = m_entityManager->EntitiesContaining("Region", m_componentMask);
}

string RegionSystem::Name()
{
	return "Region";
}
