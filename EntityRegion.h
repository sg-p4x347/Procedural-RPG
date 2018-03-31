#pragma once
#include "Component.h"
#include "Map.h"
namespace Components {
	class EntityRegion :
		public Component
	{
	public:
		EntityRegion(int regionWidth = 0);
		vector<unsigned int> Entities;
		int RegionWidth;
		// Inherited via Component
		virtual string GetName() override;
		void AddEntity(unsigned int entity);
	protected:
		virtual void Export(ofstream & ofs) override;
		virtual void Import(ifstream & ifs) override;
	};
}

