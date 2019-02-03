#pragma once
#include "JsonParser.h"
class JSON
{
public: 
	virtual void Import(JsonParser & json) = 0;
	virtual JsonParser Export() = 0;
};

