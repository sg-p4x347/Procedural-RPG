#pragma once
#include "JSON.h"

struct Entity : JSON {
	public:
		Entity(vector<string> & components);
		unsigned int ID;
		byte ComponentMask[1];
		vector<string> Components;
		// Inherited via JSON
		virtual void Import(JsonParser & jp) override;
		virtual JsonParser Export() override;

};

