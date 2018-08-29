#include "pch.h"
#include "ModelAsset.h"

ModelAsset::ModelAsset(int lodSpacing, int lodCount, string effect) : LodSpacing(lodSpacing), LodCount(lodCount), Effect(effect)
{

}

string ModelAsset::GetName()
{
	return "ModelAsset";
}

void ModelAsset::Import(std::istream & ifs)
{
	JsonParser jp = JsonParser(ifs);
	Import(jp);
}

void ModelAsset::Export(std::ostream & ofs)
{
	Export().Export(ofs);
}

void ModelAsset::Import(JsonParser & jp)
{
	LodSpacing = (int)jp["LodSpacing"];
	LodCount = (int)jp["LodCount"];
	Effect = (string)jp["Effect"];

}

JsonParser ModelAsset::Export()
{
	JsonParser jp;
	jp.Set("LodSpacing", LodSpacing);
	jp.Set("LodCount", LodCount);
	jp.Set("Effect", Effect);
	return jp;
}
