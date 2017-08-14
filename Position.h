#pragma once
#include "Position.h"
#include "Component.h"

using namespace DirectX;

struct Position : Component
{
	Position();
	Position(const unsigned int id, SimpleMath::Vector3 pos, SimpleMath::Vector3 rot);
	Position(std::ifstream & ifs);
	// Position (x,y,z)
	SimpleMath::Vector3 Pos;
	// Rotation (x,y,z)
	SimpleMath::Vector3 Rot;

	// override base serialization
	virtual void Import(std::ifstream & ifs) override;
	virtual void Export(std::ofstream & ofs) override;

	vector<Position> & GetComponents();
	// Inherited via Component
	virtual shared_ptr<Component> GetComponent(const unsigned int & id) override;
	virtual void AddComponent(const unsigned int & id) override;
	virtual void Save(string directory) override;
	virtual void Load(string directory) override;
};

