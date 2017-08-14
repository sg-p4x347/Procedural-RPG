#pragma once
#include "Component.h"
class Input :
	public Component
{
public:
	Input();
	~Input();

	vector<Input> & GetComponents();

	// Inherited via Component
	virtual Component * GetComponent(const unsigned int & id) override;
	virtual void AddComponent(const unsigned int & id) override;
	virtual void Save(string directory) override;
	virtual void Load(string directory) override;
};

