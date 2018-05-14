#pragma once
#include "Component.h"
#include "JSON.h"
class NormalMapAsset :
	public Components::Component,
	public JSON
{
public:
	NormalMapAsset();
	NormalMapAsset(unsigned int xsize, unsigned int ysize);
	// Data
	unsigned int Xsize;
	unsigned int Ysize;
	// Inherited via Component
	virtual string GetName() override;

	// Inherited via JSON
	virtual void Import(JsonParser & jp) override;
	virtual JsonParser Export() override;
	
};

