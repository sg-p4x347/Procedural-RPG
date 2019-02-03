#pragma once
#include "JSON.h"
#include "JsonParser.h"
#include "Component.h"
#include "Map.h"
class ModelAsset :
	public Components::Component,
	public JSON
{
public:
	ModelAsset(int lodSpacing = 1, int lodCount = 1, string effect = "");
	// Data
	int LodSpacing; // how far apart each LOD is
	int LodCount; // how many LOD levels are there

	string Effect;
	vector<shared_ptr<Model>> LODs; // indexed by LOD level
	Map<std::pair<int,shared_ptr<Model>>> RegionModels; // For heightmaps
	// Inherited via Component
	virtual string GetName() override;
	virtual void Import(std::istream & ifs) override;
	virtual void Export(std::ostream & ofs) override;
	// JSON
	virtual void Import(JsonParser & jp) override;
	virtual JsonParser Export() override;
};

