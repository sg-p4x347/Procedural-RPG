#pragma once
#include "Bank.h"
#include "Account.h"

// Private banks (state and local)
class PrivateBank : 
	public Bank 
{
public:
	PrivateBank(Account * account);
	void Update();
private:
	Account * m_account;
};

