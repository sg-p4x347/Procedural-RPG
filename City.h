#pragma once
#include "Component.h"
#include "JsonParser.h"

namespace Components {
	// Defines cities that lie on the continent
	class City : public Component {
	public:
		City(const unsigned int & id);
		City(const unsigned int & id, string name, DirectX::SimpleMath::Rectangle area);
		~City();
		//----------------------------------------------------------------
		// Getters
		string GetCityName();
		DirectX::SimpleMath::Rectangle GetArea();
		vector<unsigned int> GetBuildings();

		// Inherited via Component
		virtual void Import(std::ifstream & ifs) override;
		virtual void Export(std::ofstream & ofs) override;
		virtual string GetName() override;
	private:
		// Meta information
		string m_name;
		DirectX::SimpleMath::Rectangle m_area;
		vector<unsigned int> m_citizens;
		vector<unsigned int> m_buildings;


		

	};
}