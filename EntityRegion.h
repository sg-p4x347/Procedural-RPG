#pragma once
#include "Component.h"
#include "Map.h"
namespace Components {
	class EntityRegion :
		public Component
	{
	public:
		EntityRegion(int regionWidth);
		vector<unsigned int> Entities;
		int RegionWidth;
		// Inherited via Component
		virtual string GetName() override;
		//----------------------------------------------------------------
		// Sector operations
		//void AddEntity(EntityPtr entity);
		//vector<unsigned int> FindEntities(Vector3 center, float range);
		//void MoveEntity(EntityPtr entity, Vector3 lastPos);
	protected:
		virtual void Export(ofstream & ofs) override;
		virtual void Import(ifstream & ifs) override;
	private:
		static const int m_sectorDivisions;
		/*Sectors provide a fine grained lookup of entities by position*/
		Map<vector<unsigned int>> m_sectors;
		vector<unsigned int> m_sector
	};
}

