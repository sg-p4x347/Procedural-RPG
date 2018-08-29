#pragma once
#include "Component.h"
#include "JSON.h"
class PathID :
	public Components::Component,
	public JSON
{
public:
	PathID();
	PathID(string path);
	// Data
	string Path;
	// Inherited via Component
	virtual string GetName() override;
	virtual void Import(std::istream & ifs) override;
	virtual void Export(std::ostream & ofs) override;
	// Inherited via JSON
	virtual void Import(JsonParser & jp) override;
	virtual JsonParser Export() override;
};

