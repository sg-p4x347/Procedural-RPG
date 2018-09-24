#include "pch.h"
#include "Model.h"
namespace world {
	Model::Model()
	{
	}
	Model::Model(
		EntityID asset, 
		AssetType type) :
		Asset(asset),
		Type(type)
	{

	}

	/*void Model::Export(std::ostream & ofs)
	{
		Component::Export(ofs);
		Serialize(Path, ofs);
		Serialize(Type, ofs);
	}
	void Model::Import(std::istream & ifs)
	{
		Component::Import(ifs);
		DeSerialize(Path, ifs);
		DeSerialize(Type, ifs);
	}*/
}
