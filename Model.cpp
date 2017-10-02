#include "pch.h"
#include "ModelManager.h"
#include "Model.h"

namespace Component {
	Model::Model(const unsigned int & id) : Component(id)
	{
	}

	Model::Model(std::ifstream & ifs)
	{
		Import(ifs);
	}

	void Model::Import(std::ifstream & ifs)
	{
		DeSerialize(ModelID, ifs);
		DeSerialize(TextureID, ifs);
	}

	void Model::Export(std::ofstream & ofs)
	{
		Serialize(ModelID, ofs);
		Serialize(TextureID, ofs);
	}


	vector<Model>& Model::GetComponents()
	{
		static vector<Model> components;
		return components;
	}

	shared_ptr<Component> Model::GetComponent(const unsigned int & id)
	{
		for (Model & physical : GetComponents()) {
			if (physical.ID == id) {
				return std::make_shared<Component>(physical);
			}
		}
		return nullptr;
	}

	void Model::Attach(const unsigned int & id)
	{
		GetComponents().push_back(Model(id));
	}

	void Model::Save(string directory)
	{
		std::ofstream ofs(directory + "/Position.dat");
		if (ofs) {
			for (Model & position : GetComponents()) {
				position.Export(ofs);
			}
		}
	}

	void Model::Load(string directory)
	{
		std::ifstream ifs(directory + "/Position.dat");
		while (ifs) {
			GetComponents().push_back(Model(ifs));
		}
	}

}
