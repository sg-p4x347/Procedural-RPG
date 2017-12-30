#include "pch.h"
#include "Tag.h"
namespace Components {
	Tag::Tag(const unsigned int & id, string name) : Component::Component(id), Name(name)
	{
	}

	Tag::~Tag()
	{
	}

	string Tag::GetName()
	{
		return "Tag_" + Name;
	}
}
