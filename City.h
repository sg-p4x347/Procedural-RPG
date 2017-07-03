#pragma once
#include <DirectXMath.h>
#include <vector>
#include <map>

#include "SimpleMath.h"
#include "Road.h"
#include "Building.h"
#include "HeightMap.h"
#include "JSON.h"

using namespace DirectX;
using namespace SimpleMath;
using namespace std;

// Defines cities that lie on the continent
 class City : public JSON {
 public:
	 City();
	 City(JsonParser & city);
	 City(SimpleMath::Rectangle area,const HeightMap & terrain, const HeightMap & biome);
	 ~City();
	 Vector2 GetPosition();
	 SimpleMath::Rectangle GetArea();
	 vector<shared_ptr<Architecture::Building>> GetBuildings();
	 void SaveCity(string directory);

	 // Graphics
	 void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
	 // Inherited via JSON
	 virtual void Import(JsonParser & jp) override;
	 virtual JsonParser Export() override;
 private:
	 // Meta information
	 string m_name;
	 SimpleMath::Rectangle m_area;
	 vector<long> m_citizens;
	 // city construction
	 vector<Road> m_streets;
	 vector<shared_ptr<Architecture::Building>> m_buildings;
	 // biome information
	 map<string, float> m_biomes;
	 // return the name of the biome that this sample point lies in
	 string GetBiomeName(float value);

	 
 };