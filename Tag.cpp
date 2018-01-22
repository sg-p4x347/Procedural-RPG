#include "pch.h"
#include "Tag.h"
namespace Components {
	Tag::Tag(string name) : Delegate::Delegate(name)
	{
	}

	string Tag::GetDiscreteName()
	{
		return "Tag";
	}
}
