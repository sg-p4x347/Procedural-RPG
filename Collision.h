#pragma once
#include "Component.h"
#include "Box.h"
namespace Components {
	class Collision :
		public Component
	{
	public:
		Collision();
		Collision(Box box);
		// Data
		Box BoundingBox;
		bool Enabled;
		// Inherited via Component
		virtual string GetName() override;
		virtual void Import(std::istream & ifs) override;
		virtual void Export(std::ostream & ofs) override;
	};
}

