#include "pch.h"
#include "Component.h"
#include "JsonParser.h"

using namespace Filesystem;

namespace Components {

	Component::Component(const unsigned int & id) : ID(id) {}

	std::ofstream Component::GetOutputStream(string directory, const unsigned int & id)
	{
		return std::ofstream(directory+ '\\' + GetName() + '\\' + std::to_string(id) + ".dat");
	}
	std::ifstream Component::GetInputStream(string directory, const unsigned int & id)
	{
		return std::ifstream(directory + '\\' + GetName() + '\\' + std::to_string(id) + ".dat");
	}

	Component::Component()
	{
	}

	Component::Component(Component & other) : Component::Component(other.ID)
	{
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
		std::ifstream ifs = GetInputStream(directory, id);
		if (ifs) {
			shared_ptr<Component> component = Add(id);
			component->Import(ifs);
			ifs.close();
			return component;
		}
		return nullptr;
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
	}
	void Component::Export(std::ofstream & ofs)
	{
	}
}





