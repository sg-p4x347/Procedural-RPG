#pragma once
#include "Component.h"
#include "JSON.h"
class HeightMapAsset :
	public Components::Component,
	public JSON
{
public:
	HeightMapAsset();
	HeightMapAsset(unsigned int xsize,unsigned int ysize, float scaleFactor = 1.f, unsigned int regionWidth = 2);
	// Data
	unsigned int Xsize;
	unsigned int Ysize;
	float ScaleFactor;
	unsigned int RegionWidth;
	// Inherited via Component
	virtual string GetName() override;
	virtual void Import(std::istream & ifs) override;
	virtual void Export(std::ostream & ofs) override;
	// Inherited via JSON
	virtual void Import(JsonParser & json) override;
	virtual JsonParser Export() override;
};

