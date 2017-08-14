#include "pch.h"
#include "ModelManager.h"
#include "Physical.h"


Physical::Physical(const unsigned int & id) : Component(id)
{
}

Physical::Physical(std::ifstream & ifs)
{
	Import(ifs);
}

void Physical::Import(std::ifstream & ifs)
{
	DeSerialize(ModelID, ifs);
	DeSerialize(TextureID, ifs);
}

void Physical::Export(std::ofstream & ofs)
{
	Serialize(ModelID, ofs);
	Serialize(TextureID, ofs);
}


vector<Physical>& Physical::GetComponents()
{
	static vector<Physical> components;
	return components;
}

shared_ptr<Component> Physical::GetComponent(const unsigned int & id)
{
	for (Physical & physical : GetComponents()) {
		if (physical.ID == id) {
			return std::make_shared<Component>(physical);
		}
	}
	return nullptr;
}

void Physical::AddComponent(const unsigned int & id)
{
	GetComponents().push_back(Physical(id));
}

void Physical::Save(string directory)
{
	std::ofstream ofs(directory + "/Position.dat");
	if (ofs) {
		for (Physical & position : GetComponents()) {
			position.Export(ofs);
		}
	}
}

void Physical::Load(string directory)
{
	std::ifstream ifs(directory + "/Position.dat");
	while (ifs) {
		GetComponents().push_back(Physical(ifs));
	}
}


