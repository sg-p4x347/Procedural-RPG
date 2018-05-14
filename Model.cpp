#include "pch.h"
#include "Model.h"
namespace Components {
	Model::Model()
	{
	}
	Model::Model(
		string path, 
		AssetType type) :
		Path(path),
		Type(type)
	{

	}

	string Model::GetName()
	{
		return "Model";
	}
	void Model::Export(std::ofstream & ofs)
	{
		Component::Export(ofs);
		Serialize(Path, ofs);
		Serialize(Type, ofs);
	}
	void Model::Import(std::ifstream & ifs)
	{
		Component::Import(ifs);
		DeSerialize(Path, ifs);
		DeSerialize(Type, ifs);
	}
}
