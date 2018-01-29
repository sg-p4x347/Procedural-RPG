#pragma once
#include "JSON.h"
#include "JsonParser.h"
#include "Component.h"
class ModelAsset :
	public Components::Component,
	public JSON
{
public:
	ModelAsset();
	ModelAsset(string path, int lodSpacing = 1, int lodCount = 1);
	// Data
	string Path;
	int LodSpacing; // how far apart each LOD is
	int LodCount; // how many LOD levels are there
	vector<shared_ptr<Model>> LODs; // indexed by LOD level
	// Inherited via Component
	virtual string GetName() override;
	virtual void Import(std::ifstream & ifs) override;
	virtual void Export(std::ofstream & ofs) override;
	// JSON
	virtual void Import(JsonParser & jp) override;
	virtual JsonParser Export() override;
};

