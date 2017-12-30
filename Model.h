#pragma once
#include "Component.h"
namespace Components {
	class Model :
		public Component
	{
	public:
		Model(const unsigned int id);
		Model(const unsigned int id,string path,string effect, bool procedural = false);
		~Model();
		
		// Relative path for locating the model
		string Path;
		// Determines which top level 'Assets' folder is used
		bool Procedural;
		// Effect used to render the model
		string Effect;

		// Inherited via Component
		virtual string GetName() override;
		virtual void Export(std::ofstream & ofs) override;
		virtual void Import(std::ifstream & ifs) override;
	};
}