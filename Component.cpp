#include "pch.h"
#include "Component.h"
#include "JsonParser.h"

Component::Component() : Component(0)
{
}

Component::Component(unsigned int id) : ID(id)
{

}

Component::Component(std::ifstream & ifs)
{
	Import(ifs);
}


Component::~Component()
{
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



