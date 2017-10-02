#include "pch.h"
#include "Component.h"
#include "JsonParser.h"

using namespace Filesystem;

Component::Component() {}


Component::Component(std::ifstream & ifs)
{
	Import(ifs);
}

Component::Component(const unsigned int & id) : ID(id) {}

std::ofstream Component::GetOutputStream(string directory, const unsigned int & id)
{
	return std::ofstream(CombinePath(CombinePath(directory, GetName()), id + ".dat"));
}
std::ifstream Component::GetInputStream(string directory, const unsigned int & id)
{
	return std::ifstream(CombinePath(CombinePath(directory, GetName()), id + ".dat"));
}

void Component::Save(string directory)
{
	std::ofstream ofs = GetOutputStream(directory, ID);
	if (ofs) {
		Export(ofs);
		ofs.close();
	}
}
shared_ptr<Component> Component::Load(string directory, const unsigned int & id)
{
	std::ifstream ifs = GetInputStream(directory,id);
	if (ifs) {
		shared_ptr<Component> component = Create(ifs);
		ifs.close();
		Attach(component);
		return component;
	}
	return shared_ptr<Component>(nullptr);
}

// JSON
void Component::Import(JsonParser & obj)
{
	ID = (unsigned int)obj["id"];
}
JsonParser Component::Export()
{
	JsonParser obj;
	obj.Set("id", ID);
	return obj;
}
// ISerialization
void Component::Import(std::ifstream & ifs)
{
	DeSerialize(ID, ifs);
}
void Component::Export(std::ofstream & ofs)
{
	Serialize(ID, ofs);
}





