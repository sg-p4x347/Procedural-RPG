#include "pch.h"
#include "Position.h"

Position::Position() : Component()
{
}

Position::Position(const unsigned int id, SimpleMath::Vector3 pos, SimpleMath::Vector3 rot) : Component(id)
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
	Pos = SimpleMath::Vector3(x, y, z);

	float xRot, yRot, zRot;
	DeSerialize(xRot, ifs);
	DeSerialize(yRot, ifs);
	DeSerialize(zRot, ifs);
	Rot = SimpleMath::Vector3(xRot, yRot, zRot);
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

shared_ptr<Component> Position::GetComponent(const unsigned int & id)
{

	for (Position & position : GetComponents()) {
		if (position.ID == id) {
			return std::make_shared<Component>(&position);
		}
	}
	return nullptr;
}

void Position::AddComponent(const unsigned int & id)
{
	GetComponents().push_back(Position(id,SimpleMath::Vector3::Zero, SimpleMath::Vector3::Zero));
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
