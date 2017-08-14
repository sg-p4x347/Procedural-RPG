#pragma once
#include "ISerialization.h"
#include "JSON.h"

using string = std::string;
struct Component : ISerialization, JSON
{
public:
	// Constructor & Destructor
	Component();
	Component(unsigned int id);
	Component(std::ifstream & ifs);
	~Component();

	// Abstract component accessors
	virtual shared_ptr<Component> GetComponent(const unsigned int & id) = 0;
	virtual void AddComponent(const unsigned int & id) = 0;
	virtual void Save(string directory) = 0;
	virtual void Load(string directory) = 0;

	// Helper
	// The ID of the entity this belongs to
	unsigned int ID;

protected:
	// Inerited via ISerialization
	virtual void Import(std::ifstream & ifs) override;
	virtual void Export(std::ofstream & ofs) override;

	// Inherited via JSON
	virtual void Import(JsonParser & jp) override;
	virtual JsonParser Export() override;
};

