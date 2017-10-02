#pragma once
#include "Position.h"
#include "Component.h"

using namespace DirectX;

namespace Component {
	class Position : public Component
	{
	public:

		Position();
		Position(const unsigned int & id);
		Position(const unsigned int id, SimpleMath::Vector3 pos, SimpleMath::Vector3 rot);

		// Position (x,y,z)
		SimpleMath::Vector3 Pos;
		// Rotation (x,y,z)
		SimpleMath::Vector3 Rot;

		// Prototype
		virtual shared_ptr<Component> GetComponent(const unsigned int & id) override;
		virtual void SaveAll(string directory) override;

		virtual string GetName() override;
	protected:
		Position(std::ifstream & ifs);
		// Inherited via Component
		virtual shared_ptr<Component> Create(std::ifstream & ifs) override;
		virtual void Attach(shared_ptr<Component> component) override;

		// override base serialization
		virtual void Import(std::ifstream & ifs) override;
		virtual void Export(std::ofstream & ofs) override;

		vector<Position> & GetComponents();
	};
}

