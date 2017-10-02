#include "pch.h"
#include "Position.h"

namespace Component {
	Position::Position() : Component()
	{
		Pos = SimpleMath::Vector3::Zero;
		Rot = SimpleMath::Vector3::Zero;
	}

	Position::Position(const unsigned int & id) : Position()
	{
		ID = id;
	}

	Position::Position(const unsigned int id, SimpleMath::Vector3 pos, SimpleMath::Vector3 rot) : Component(id)
	{
		Pos = pos;
		Rot = rot;
	}


	Position::Position(std::ifstream & ifs)
	{
		Import(ifs);
	}

	shared_ptr<Component> Position::Create(std::ifstream & ifs)
	{
		return shared_ptr<Component>(new Position(ifs));
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

	shared_ptr<Component> Position::GetComponent(const unsigned int & id)
	{
		// Query in-memory list
		for (Position & position : GetComponents()) {
			if (position.ID == id) {
				return std::make_shared<Component>(&position);
			}
		}
		// Not found
		return std::make_shared<Component>(nullptr);
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

	void Position::Attach(shared_ptr<Component> component)
	{
		GetComponents().push_back(*dynamic_cast<Position*>(component.get()));
	}
}
