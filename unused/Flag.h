#pragma once
#include "Component.h"
class Flag :
	public Component
{
public:
	Flag(string flagName);
	~Flag();

	// Inherited via Component
	virtual shared_ptr<Component> GetComponent(const unsigned int & id) override;
	virtual void Attach(const unsigned int & id) override;
	virtual void Save(string directory) override;
	virtual void Load(string directory) override;
};

