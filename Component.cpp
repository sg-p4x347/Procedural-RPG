#include "pch.h"
#include "Component.h"
#include "JsonParser.h"

using namespace Filesystem;

namespace Components {
	Component::Component(const unsigned int & id) : ID(id) {}
	Filesystem::path Component::m_directory;
	std::ofstream Component::GetOutputStream(Filesystem::path directory, const unsigned int & id)
	{
		Filesystem::path componentDir = directory / GetName();
		Filesystem::create_directory(componentDir);
		return std::ofstream(componentDir / (std::to_string(id) + ".dat"));
	}
	std::ifstream Component::GetInputStream(Filesystem::path directory, const unsigned int & id)
	{
		Filesystem::path componentDir = directory / GetName();
		Filesystem::create_directory(componentDir);
		return std::ifstream(componentDir / (std::to_string(id) + ".dat"));
	}

	Component::Component(Component & other) : Component::Component(other.ID)
	{
	}

	Component::~Component()
	{
	}

	void Component::SetDirectory(Filesystem::path directory)
	{
		m_directory = directory;
	}

	Filesystem::path Component::GetDirectory()
	{
		return m_directory;
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

	// ISerialization
	void Component::Import(std::ifstream & ifs)
	{
	}
	void Component::Export(std::ofstream & ofs)
	{
	}
}





