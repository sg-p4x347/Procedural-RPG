#pragma once
#include "Component.h"

class System
{
public:
	System(vector<string> & components);
	~System();
private:
	vector<string> m_components;
};

