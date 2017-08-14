#pragma once
#include "Component.h"
class Physical :
	public Component
{
public:
	Physical(const unsigned int & id);
	Physical(std::ifstream & ifs);
	
	// override base serialization
	virtual void Import(std::ifstream & ifs) override;
	virtual void Export(std::ofstream & ofs) override;

	vector<Physical> & GetComponents();
	// Inherited via Component
	virtual shared_ptr<Component> GetComponent(const unsigned int & id) override;
	virtual void AddComponent(const unsigned int & id) override;
	virtual void Save(string directory) override;
	virtual void Load(string directory) override;

	// Model
	unsigned short ModelID;
	// Texture
	unsigned short TextureID;
};

