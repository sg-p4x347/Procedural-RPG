#include "pch.h"
#include "Position.h"

namespace Components {
	Position::Position(const Position & other) : Position::Position(other.Pos,other.Rot)
	{
	}


	Position::Position(SimpleMath::Vector3 pos, SimpleMath::Vector3 rot)
	{
		Pos = pos;
		Rot = rot;
	}


	void Position::Import(std::istream & ifs)
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

	void Position::Export(std::ostream & ofs)
	{
		Component::Export(ofs);
		Serialize(Pos.x, ofs);
		Serialize(Pos.y, ofs);
		Serialize(Pos.z, ofs);

		Serialize(Rot.x, ofs);
		Serialize(Rot.y, ofs);
		Serialize(Rot.z, ofs);
	}
	string Position::GetName()
	{
		return "Position";
	}
}
