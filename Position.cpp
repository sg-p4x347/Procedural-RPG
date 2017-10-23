#include "pch.h"
#include "Position.h"

namespace Components {
	Position::Position() : Component::Component()
	{
	}
	Position::Position(const Position & other) : Position::Position(other.ID,other.Pos,other.Rot)
	{
	}
	Position::Position(const unsigned int & id) : Position::Position(id, SimpleMath::Vector3::Zero,SimpleMath::Vector3::Zero)
	{
	}


	Position::Position(const unsigned int id, SimpleMath::Vector3 pos, SimpleMath::Vector3 rot) : Component::Component(id)
	{
		Pos = pos;
		Rot = rot;
	}


	void Position::Import(std::ifstream & ifs)
	{
		Component::Import(ifs);
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
		Component::Export(ofs);
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

	shared_ptr<Component> Position::Add(const unsigned int & id)
	{
		Position component = Position(id);
		GetComponents().push_back(component);
		return std::shared_ptr<Component>(&component);
	}

	shared_ptr<Component> Position::GetComponent(const unsigned int & id)
	{
		// Query in-memory list
		for (Position & position : GetComponents()) {
			if (position.ID == id) {
				return std::shared_ptr<Component>(&position);
			}
		}
		// Not found
		return nullptr;
	}

	void Position::SaveAll(string directory)
	{
		for (Position & position : GetComponents()) {
			position.Save(directory);
		}
	}

	string Position::GetName()
	{
		return "Position";
	}
}
