#include "pch.h"
#include "City.h"
#include "NameGenerator.h"


City::City(XMFLOAT3 position,const vector<vector<float>> & terrain) {
	m_position = position;
	// generate name
	NameGenerator NG;
	m_name = NG.getCityName();
	
}

City::~City() {
}

XMFLOAT3 City::GetPosition()
{
	return m_position;
}
