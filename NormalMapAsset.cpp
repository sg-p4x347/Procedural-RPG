#include "pch.h"
#include "NormalMapAsset.h"


NormalMapAsset::NormalMapAsset()  : NormalMapAsset::NormalMapAsset(0,0)
{
}

NormalMapAsset::NormalMapAsset(unsigned int xsize, unsigned int ysize) : Xsize(xsize), Ysize(ysize)
{
}

string NormalMapAsset::GetName()
{
	return "NormalMapAsset";
}

void NormalMapAsset::Import(JsonParser & jp)
{
	Xsize = (unsigned int)jp["Xsize"];
	Ysize = (unsigned int)jp["Ysize"];
}

JsonParser NormalMapAsset::Export()
{
	JsonParser jp;
	jp.Set("Xsize", Xsize);
	jp.Set("Ysize", Ysize);
	return jp;
}
