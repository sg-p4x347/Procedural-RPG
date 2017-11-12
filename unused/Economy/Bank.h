#pragma once
#include "Account.h"

class Bank {

public:
	
	Bank();
	Account * AddAccount();
	virtual void Update() = 0;
protected:
	std::vector<Account *> m_accounts;
	float m_interestRate;
	float m_currency; // ammount of money the bank has
};

