#include "pch.h"
#include "Component.h"
#include "JsonParser.h"

using namespace Filesystem;

namespace Components {
	Component::Component() : ID(0) {}
	Component::Component(const unsigned int & id) : ID(id) {}

	Component::Component(Component & other) : Component::Component(other.ID)
	{
	}

	Component::~Component()
	{
	}

	shared_ptr<Components::Component> Component::Copy(shared_ptr<Components::Component> source)
	{
		return nullptr;
	}

	// ISerialization
	void Component::Import(std::istream & ifs)
	{
	}
	void Component::Export(std::ostream & ofs)
	{
	}
}





