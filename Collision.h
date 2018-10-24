#pragma once
#include "WorldComponent.h"
#include "Box.h"
namespace world {
	class Collision :
		public WorldComponent
	{
	public:
		Collision();
		Collision(Box box);
		// Data
		Box BoundingBox;
		bool Enabled;
		int Colliding;
		std::vector<Vector3> CollisionNormals;
		// Inherited via Component
		//virtual void Import(std::istream & ifs) override;
		//virtual void Export(std::ostream & ofs) override;
	};
}

