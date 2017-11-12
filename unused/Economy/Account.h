#pragma once

#include <list>
#include <vector>

#include "Loan.h"

struct Account {
	
	// Update account
	void Update(float IR);
	// attempts to pay off all debt
	// also logs debt paid on running total
	void Compensate(float & ammount);
	std::vector<Loan> loans = std::vector<Loan>();
	float income = 0;				// the amount of income in the past year
	float debt = 0;					// the cumulative amount of outstanding currency owed
	float debtPaid = 0;
	float interestPaid = 0.f;			// the amount of interest paid in this year
	float totalDebtAquired = 0;		// running total of debt taken on
	float totalDebtPaid = 0;		// running total of debt paid
	float credit = 0;				// amount of currency that can be borrowed
	float initialCredit = 0;		// compared with credit to determine debt
	float interestRate = 0.f;		// percentage of payment that goes to the bank
	std::list<float> m_creditHistory = std::list<float>();
};

