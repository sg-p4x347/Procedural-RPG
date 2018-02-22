#pragma once
#include "Bank.h"
// Federal bank (monetary issuer and authority)
class FederalBank : 
	public Bank 
{
public:
	FederalBank();
	void Update();
	void Inflate(float stimulus);
};

