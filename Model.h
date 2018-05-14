#pragma once
#include "Component.h"
#include "AssetTypes.h"
namespace Components {
	
	class Model :
		public Component
	{
	public:
		Model();
		Model(string path,AssetType type);
		
		// Relative path for locating the model
		string Path;
		// Determines how to import this model
		AssetType Type;

		//// Effect used to render the model
		//string Effect;
		//// Cull back faces
		//bool BackfaceCulling;

		// Inherited via Component
		virtual string GetName() override;
		virtual void Export(std::ofstream & ofs) override;
		virtual void Import(std::ifstream & ifs) override;
	};
}