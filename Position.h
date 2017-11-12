#pragma once
#include "Position.h"
#include "Component.h"

using namespace DirectX;

namespace Components {
	class Position : public Component
	{
	public:
		Position(const Position & other);
		Position(const unsigned int & id);
		Position(const unsigned int id, SimpleMath::Vector3 pos, SimpleMath::Vector3 rot);
		~Position();
		// Position (x,y,z)
		SimpleMath::Vector3 Pos;
		// Rotation (x,y,z)
		SimpleMath::Vector3 Rot;
		
		// Inherited via Component
		virtual string GetName() override;
	protected:
		// override base serialization
		virtual void Import(std::ifstream & ifs) override;
		virtual void Export(std::ofstream & ofs) override;
	};
}

