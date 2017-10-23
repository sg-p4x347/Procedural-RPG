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

	void Model::Attach(shared_ptr<Component>& component)
	{
		GetComponents().push_back(*dynamic_cast<Model*>(component.get()));
	}

	void Model::SaveAll(string directory)
	{
		for (Model & model : GetComponents()) {
			model.Save(directory);
		}
	}
}
