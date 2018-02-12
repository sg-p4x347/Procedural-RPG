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
}
