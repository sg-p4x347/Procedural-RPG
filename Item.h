#pragma once
#include "Component.h"
#include "JSON.h"
class Item :
	public Components::Component,
	public JSON
{
public:
	Item();
	// Data
	string Name;
	string Description;
	float Mass;
	string Sprite;

	// Inherited via Component
	virtual string GetName() override;
	void Import(std::istream & ifs) override;
	void Export(std::ostream & ofs) override;

	// Inherited via JSON
	virtual void Import(JsonParser & jp) override;
	virtual JsonParser Export() override;
};

