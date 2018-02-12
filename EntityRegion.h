#pragma once
#include "Component.h"
namespace Components {
	class EntityRegion :
		public Component
	{
	public:
		EntityRegion();
		vector<unsigned int> Entities;

		// Inherited via Component
		virtual string GetName() override;

	protected:
		virtual void Export(ofstream & ofs) override;
		virtual void Import(ifstream & ifs) override;
	};
}

