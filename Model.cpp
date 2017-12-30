#include "pch.h"
#include "Model.h"
namespace Components {
	Model::Model(const unsigned int id) : Component::Component(id)
	{
	}
	Model::Model(const unsigned int id, string path, string effect, bool procedural) : Component::Component(id), Path(path),Procedural(procedural),Effect(effect)
	{

	}

	Model::~Model()
	{
		Save(Component::GetDirectory());
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
