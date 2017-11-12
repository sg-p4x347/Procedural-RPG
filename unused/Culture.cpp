#include "pch.h"
#include "Culture.h"
#include "NameGenerator.h"
#include "Utility.h"

using namespace Utility;

Culture::Culture() {
	NameGenerator ng = NameGenerator();
	name = ng.GetRegionName();
	holistic = Vector3();
	//  - Investigative <-> Enterprising +
	leadership = randWithin(0.f, 1.f);
	accountability = randWithin(0.f, 1.f);
	empowerment = randWithin(0.f, 1.f);
	float enterprising = (leadership + accountability + empowerment) / 3.f;
	adventure = randWithin(0.f, 1.f);
	spirituality = randWithin(0.f, 1.f);
	freedom = randWithin(0.f, 1.f);
	float investigative = (adventure + spirituality + freedom) / 3.f;
	holistic.x = enterprising - investigative;
	// - Artistic <-> Conventional +
	accuracy = randWithin(0.f, 1.f);
	order = randWithin(0.f, 1.f);
	efficiency = randWithin(0.f, 1.f);
	float conventional = (accuracy + order + efficiency) / 3.f;
	creativity = randWithin(0.f, 1.f);
	expression = randWithin(0.f, 1.f);
	documentation = randWithin(0.f, 1.f);
	float artistic = (creativity + expression + documentation) / 3.f;
	holistic.y = conventional - artistic;
	// - Social <-> Realistic +
	occupation = randWithin(0.f, 1.f);
	skill = randWithin(0.f, 1.f);
	hardWork = randWithin(0.f, 1.f);
	float realistic = (occupation + skill + hardWork) / 3.f;
	compassion = randWithin(0.f, 1.f);
	family = randWithin(0.f, 1.f);
	tolerance = randWithin(0.f, 1.f);
	float social = (compassion + family + tolerance) / 3.f;
	holistic.z = realistic - social;

	// Environment
	plants = randWithin(0.f, 1.f);
	animals = randWithin(0.f, 1.f);
	earth = randWithin(0.f, 1.f);
}
