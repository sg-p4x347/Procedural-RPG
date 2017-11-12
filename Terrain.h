#pragma once
#include "Component.h"
#include "HeightMap.h"
#include "VectorMap.h"

namespace Components {
	class Terrain :
		public Component
	{
	public:
		Terrain(const Terrain & other);
		Terrain(const unsigned int & id);

		// Data
		/*HeightMap TerrainMap;
		VectorMap NormalMap;
		HeightMap BiomeMap;*/
		
		// Inherited via Component
		virtual string GetName() override;
	protected:
		virtual void Import(std::ifstream & ifs) override;
		virtual void Export(std::ofstream & ofs) override;
	};
}

