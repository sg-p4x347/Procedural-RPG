#include "pch.h"
#include "PathID.h"


PathID::PathID() : PathID::PathID("")
{
}

PathID::PathID(string path) : Path(path)
{
}

string PathID::GetName()
{
	return "PathID";
}

void PathID::Import(std::istream & ifs)
{
	JsonParser jp(ifs);
	Import(jp);
}

void PathID::Export(std::ostream & ofs)
{
	Export().Export(ofs);
}

void PathID::Import(JsonParser & jp)
{
	Path = (string)jp["Path"];
}

JsonParser PathID::Export()
{
	JsonParser jp;
	jp.Set("Path", Path);
	return jp;
}
