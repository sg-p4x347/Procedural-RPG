#include "pch.h"
#include "Item.h"


Item::Item()
{
}

string Item::GetName()
{
	return "Item";
}

void Item::Import(std::istream & ifs)
{
	JsonParser jp(ifs);
	Import(jp);
}

void Item::Export(std::ostream & ofs)
{
	Export().Export(ofs);
}

void Item::Import(JsonParser & jp)
{
	Name = (string)jp["Name"];
	Description = (string)jp["Description"];
	Mass = (float)jp["Mass"];
	Sprite = (string)jp["Sprite"];
}

JsonParser Item::Export()
{
	JsonParser jp;
	jp.Set("Name", Name);
	jp.Set("Description", Description);
	jp.Set("Mass", Mass);
	jp.Set("Sprite", Sprite);
	return jp;
}
