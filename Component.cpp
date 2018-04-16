#include "pch.h"
#include "Component.h"
#include "JsonParser.h"

using namespace Filesystem;

namespace Components {
	Component::Component() : ID(0) {}
	Component::Component(const unsigned int & id) : ID(id) {}
	std::ofstream Component::GetOutputStream(Filesystem::path directory, const unsigned int & id)
	{
		Filesystem::path componentDir = directory / GetName();
		Filesystem::create_directory(componentDir);
		return std::ofstream(componentDir / (std::to_string(id) + ".dat"),ios::binary);
	}
	std::ifstream Component::GetInputStream(Filesystem::path directory, const unsigned int & id)
	{
		Filesystem::path componentDir = directory / GetName();
		Filesystem::create_directory(componentDir);
		return std::ifstream(componentDir / (std::to_string(id) + ".dat"), ios::binary);
	}

	Component::Component(Component & other) : Component::Component(other.ID)
	{
	}

	Component::~Component()
	{
	}

	void Component::Save(Filesystem::path directory)
	{
		std::ofstream ofs = GetOutputStream(directory, ID);
		if (ofs) {
			Export(ofs);
			ofs.close();
		}
	}
	void Component::Load(Filesystem::path directory, const unsigned int & id)
	{
		std::ifstream ifs = GetInputStream(directory, id);
		if (ifs) {
			Import(ifs);
			ifs.close();
		}
	}

	shared_ptr<Components::Component> Component::Copy(shared_ptr<Components::Component> source)
	{
		return nullptr;
	}

	// ISerialization
	void Component::Import(std::ifstream & ifs)
	{
	}
	void Component::Export(std::ofstream & ofs)
	{
	}
}





