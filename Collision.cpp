#include "pch.h"
#include "Collision.h"

namespace world {
	Collision::Collision() : Enabled(true)
	{
	}

	Collision::Collision(Box box) : BoundingBox(box), Enabled(true), Colliding(0)
	{
	}

	/*void Collision::Import(std::istream & ifs)
	{
		DeSerialize(Enabled, ifs);

		DeSerialize(BoundingBox.Position.x, ifs);
		DeSerialize(BoundingBox.Position.y, ifs);
		DeSerialize(BoundingBox.Position.z, ifs);

		DeSerialize(BoundingBox.Size.x, ifs);
		DeSerialize(BoundingBox.Size.y, ifs);
		DeSerialize(BoundingBox.Size.z, ifs);
	}
	void Collision::Export(std::ostream & ofs)
	{
		Serialize(Enabled, ofs);

		Serialize(BoundingBox.Position.x, ofs);
		Serialize(BoundingBox.Position.y, ofs);
		Serialize(BoundingBox.Position.z, ofs);
		
		Serialize(BoundingBox.Size.x, ofs);
		Serialize(BoundingBox.Size.y, ofs);
		Serialize(BoundingBox.Size.z, ofs);

	}*/
}
