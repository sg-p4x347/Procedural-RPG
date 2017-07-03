#include "pch.h"
#include "Position.h"

Position::Position() : Component()
{
}

Position::Position(unsigned int id, XMFLOAT3 pos, XMFLOAT3 rot) : Component(id)
{
	Pos = pos;
	Rot = rot;
}

Position::Position(std::ifstream & ifs) : Component(ifs)
{
	Import(ifs);
}

void Position::Import(std::ifstream & ifs)
{
	float x, y, z;
	DeSerialize(x, ifs);
	DeSerialize(y, ifs);
	DeSerialize(z, ifs);
	Pos = XMFLOAT3(x, y, z);

	float xRot, yRot, zRot;
	DeSerialize(xRot, ifs);
	DeSerialize(yRot, ifs);
	DeSerialize(zRot, ifs);
	Rot = XMFLOAT3(xRot, yRot, zRot);
}

void Position::Export(std::ofstream & ofs)
{
	Serialize(Pos.x, ofs);
	Serialize(Pos.y, ofs);
	Serialize(Pos.z, ofs);

	Serialize(Rot.x, ofs);
	Serialize(Rot.y, ofs);
	Serialize(Rot.z, ofs);
}

vector<Position>& Position::GetComponents()
{
	static vector<Position> components;
	return components;
}

Component * Position::GetComponent(const unsigned int & id)
{

	for (Position & position : GetComponents()) {
		if (position.ID == id) {
			return &position;
		}
		else if (position.ID > id) {
			return nullptr;
		}
	}
}

void Position::AddComponent(const unsigned int & id)
{
	GetComponents().push_back(Position(id,XMFLOAT3(0,0,0),XMFLOAT3(0,0,0)));
}

void Position::Save(string directory)
{
	std::ofstream ofs(directory + "/Position.dat");
	if (ofs) {
		for (Position & position : GetComponents()) {
			position.Export(ofs);
		}
	}
}

void Position::Load(string directory)
{
	std::ifstream ifs(directory + "/Position.dat");
	while (ifs) {
		GetComponents().push_back(Position(ifs));
	}
}
