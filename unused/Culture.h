#pragma once

#include "SimpleMath.h"

using namespace std;
using namespace DirectX::SimpleMath;

struct Culture {
	// Holland Hexagon is used to summarize the core values

	Culture();
	string name;
	//Inventory inventory;
	//// Occupations

	// Enterprising (managing)
	float leadership;
	float accountability;
	float empowerment;
	// Investigative (thinking)
	float adventure;
	float spirituality;
	float freedom;

	// Conventional (number crunching)
	float accuracy;
	float order;
	float efficiency;
	// Artistic (creating)
	float creativity;
	float expression;
	float documentation;

	// Realistic (doing)
	float occupation;
	float skill;
	float hardWork;
	// Social (helping)
	float compassion;
	float family;
	float tolerance;

	Vector3 holistic;
	// Environment
	float plants;
	float animals;
	float earth;
};

