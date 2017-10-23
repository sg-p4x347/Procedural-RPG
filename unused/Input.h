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
	virtual shared_ptr<Component> GetComponent(const unsigned int & id) override;
	virtual void Attach(const unsigned int & id) override;
	virtual void Save(string directory) override;
	virtual void Load(string directory) override;
	
};

