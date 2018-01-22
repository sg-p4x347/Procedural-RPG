#include "pch.h"
#include "Model.h"
namespace Components {
	Model::Model()
	{
	}
	Model::Model(string path, string effect, bool procedural) :Path(path),Procedural(procedural),Effect(effect)
	{

	}

	string Model::GetName()
	{
		return "Model";
	}
	void Model::Export(std::ofstream & ofs)
	{
		Component::Export(ofs);
		Serialize(Procedural,ofs);
		Serialize(Path, ofs);
		Serialize(Effect, ofs);
	}
	void Model::Import(std::ifstream & ifs)
	{
		Component::Import(ifs);
		DeSerialize(Procedural, ifs);
		DeSerialize(Path, ifs);
		DeSerialize(Effect, ifs);
	}
}
