#include "pch.h"
#include "ModelAsset.h"


ModelAsset::ModelAsset()
{
}

ModelAsset::ModelAsset(string path, int lodSpacing, int lodCount) : Path(path), LodSpacing(lodSpacing), LodCount(lodCount)
{

}

string ModelAsset::GetName()
{
	return "ModelAsset";
}

void ModelAsset::Import(std::ifstream & ifs)
{
	JsonParser jp = JsonParser(ifs);
	Import(jp);
}

void ModelAsset::Export(std::ofstream & ofs)
{
	Export().Export(ofs);
}

void ModelAsset::Import(JsonParser & jp)
{
	Path = (string)jp["Path"];
	LodSpacing = (int)jp["LodSpacing"];
	LodCount = (int)jp["LodCount"];

}

JsonParser ModelAsset::Export()
{
	JsonParser jp;
	jp.Set("Path", Path);
	jp.Set("LodSpacing", LodSpacing);
	jp.Set("LodCount", LodCount);
	return jp;
}
