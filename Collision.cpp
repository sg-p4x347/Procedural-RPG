#include "pch.h"
#include "Collision.h"

namespace Components {
	Collision::Collision() : Enabled(true)
	{
	}

	Collision::Collision(Box box) : BoundingBox(box), Enabled(true)
	{
	}

	string Collision::GetName()
	{
		return "Collision";
	}
	void Collision::Import(ifstream & ifs)
	{
		DeSerialize(Enabled, ifs);

		DeSerialize(BoundingBox.Position.x, ifs);
		DeSerialize(BoundingBox.Position.y, ifs);
		DeSerialize(BoundingBox.Position.z, ifs);

		DeSerialize(BoundingBox.Size.x, ifs);
		DeSerialize(BoundingBox.Size.y, ifs);
		DeSerialize(BoundingBox.Size.z, ifs);
	}
	void Collision::Export(ofstream & ofs)
	{
		Serialize(Enabled, ofs);

		Serialize(BoundingBox.Position.x, ofs);
		Serialize(BoundingBox.Position.y, ofs);
		Serialize(BoundingBox.Position.z, ofs);
		
		Serialize(BoundingBox.Size.x, ofs);
		Serialize(BoundingBox.Size.y, ofs);
		Serialize(BoundingBox.Size.z, ofs);

	}
}
