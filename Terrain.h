#pragma once
#include "Component.h"
#include "HeightMap.h"
#include "VectorMap.h"

namespace Component {
	class Terrain :
		public Component
	{
	public:
		Terrain();
		Terrain(const unsigned int & id);
		~Terrain();

		// Data
		/*HeightMap TerrainMap;
		VectorMap NormalMap;
		HeightMap BiomeMap;*/
		
		// Inherited via Component
		virtual shared_ptr<Component> GetComponent(const unsigned int & id) override;
		virtual void SaveAll(string directory) override;
		virtual void Attach(shared_ptr<Component> component) override;
		virtual string GetName() override;
		virtual shared_ptr<Component> Create(std::ifstream & ifs) override;
	protected:
		Terrain(std::ifstream & ifs);
		vector<Terrain>& GetComponents();
		virtual void Import(std::ifstream & ifs) override;
		virtual void Export(std::ofstream & ofs) override;
	};
}

