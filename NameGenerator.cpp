#include "pch.h"
#include "NameGenerator.h"
#include "Utility.h"
using namespace std;
using namespace Utility;

NameGenerator::NameGenerator(){}

NameGenerator::~NameGenerator(){}

string NameGenerator::getMaleName() {
	return m_maleNames[randWithin(0, m_maleNames.size())];
}

string NameGenerator::getFemaleName() {
	return m_femaleNames[randWithin(0, m_femaleNames.size())];
}

string NameGenerator::getSurName() {
	return m_surNames[randWithin(0, m_surNames.size())];
}

string NameGenerator::getCityName() {
	return m_prefixes[randWithin(0, m_prefixes.size())] + m_citySuffixes[randWithin(0, m_citySuffixes.size())];
}

string NameGenerator::getRegionName() {
	return m_prefixes[randWithin(0, m_prefixes.size())] + m_regionSuffixes[randWithin(0, m_regionSuffixes.size())];
}
