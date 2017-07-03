#pragma once
#include <directxmath.h>
#include "Position.h"
#include "Component.h"

using namespace DirectX;

struct Position : Component
{
	Position();
	Position(unsigned int id, XMFLOAT3 pos, XMFLOAT3 rot);
	Position(std::ifstream & ifs);
	// Position (x,y,z)
	XMFLOAT3 Pos;
	// Rotation (x,y,z)
	XMFLOAT3 Rot; 

	// override base serialization
	virtual void Import(std::ifstream & ifs) override;
	virtual void Export(std::ofstream & ofs) override;

	static vector<Position> Components;

	vector<Position> & GetComponents();
	// Inherited via Component
	virtual Component * GetComponent(const unsigned int & id) override;
	virtual void AddComponent(const unsigned int & id) override;
	virtual void Save(string directory) override;
	virtual void Load(string directory) override;
};

