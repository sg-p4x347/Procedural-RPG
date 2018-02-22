#pragma once
#include "JSON.h"
#include "JsonParser.h"
#include "Component.h"
class SoundAsset :
	public Components::Component,
	public JSON
{
public:
	SoundAsset();
	// Data
	string Path;

	// Inherited via Component
	virtual string GetName() override;
	virtual void Import(std::ifstream & ifs) override;
	virtual void Export(std::ofstream & ofs) override;
	// Inherited via JSON
	virtual void Import(JsonParser & jp) override;
	virtual JsonParser Export() override;
};

