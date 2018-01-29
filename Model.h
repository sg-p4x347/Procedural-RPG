#pragma once
#include "Component.h"
namespace Components {
	class Model :
		public Component
	{
	public:
		Model();
		Model(string path,string effect, bool procedural = false,bool backfaceCulling = true);
		
		// Relative path for locating the model
		string Path;
		// Determines which top level 'Assets' folder is used
		bool Procedural;
		// Effect used to render the model
		string Effect;
		// Cull back faces
		bool BackfaceCulling;

		// Inherited via Component
		virtual string GetName() override;
		virtual void Export(std::ofstream & ofs) override;
		virtual void Import(std::ifstream & ifs) override;
	};
}