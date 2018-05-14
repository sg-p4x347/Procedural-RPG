#include "pch.h"
#include "HeightMapAsset.h"


HeightMapAsset::HeightMapAsset() : HeightMapAsset::HeightMapAsset(0,0)
{
}

HeightMapAsset::HeightMapAsset(
	unsigned int xsize, 
	unsigned int ysize, 
	float scaleFactor,
	unsigned int regionWidth
) : 
	Xsize(xsize), 
	Ysize(ysize),
	ScaleFactor(scaleFactor),
	RegionWidth(regionWidth)
{
}

string HeightMapAsset::GetName()
{
	return "HeightMapAsset";
}

void HeightMapAsset::Import(JsonParser & jp)
{
	Xsize = (unsigned int)jp["Xsize"];
	Ysize = (unsigned int)jp["Ysize"];
	ScaleFactor = (float)jp["ScaleFactor"];
	RegionWidth = (unsigned int)jp["VertexSize"];
}

JsonParser HeightMapAsset::Export()
{
	JsonParser data;
	data.Set("Xsize", Xsize);
	data.Set("Ysize", Ysize);
	data.Set("ScaleFactor", ScaleFactor);
	data.Set("VertexSize", RegionWidth);
	return data;
}