#include "pch.h"
#include "Account.h"


void Account::Update(float IR) {
	interestRate = IR;
	// reset delta counters
	interestPaid = 0.f;
	debtPaid = 0.f;

	float newDebt = initialCredit - credit;
	debt += newDebt;
	debt *= (interestRate + 1.f); // add interest
	totalDebtAquired += newDebt;
	/*for (Loan loan : loans) {
	loan.Update();
	totalDebt += loan.principal;
	totalDebtPaid += loan.debtPaid;
	}*/
	// calculate credit; (debt paid / debt taken on) * income
	// then do a weighted average of the historical credit values
	if (totalDebtAquired == 0) {
		if (income > 0) {
			m_creditHistory.push_front(income);
		}
		else {
			m_creditHistory.push_front(100.f);
		}

	}
	else {
		m_creditHistory.push_front((totalDebtPaid / totalDebtAquired) * income);
	}

	// keep the history a set size
	if (m_creditHistory.size() > 5) {
		m_creditHistory.pop_back();
	}
	// average
	float numerator = 0;
	float denominator = 0;
	int i = 0;
	for (std::list<float>::const_iterator iterator = m_creditHistory.begin(), end = m_creditHistory.end(); iterator != end; ++iterator) {
		float weight = (float)(i + 1) / (float)m_creditHistory.size();
		denominator += weight;
		numerator += *iterator * weight;
		i++;
	}
	initialCredit = numerator / denominator;
	credit = initialCredit;
}
// attempts to pay off all debt
// also logs debt paid on running total
void Account::Compensate(float & ammount) {
	float payment = std::min(debt, ammount);
	ammount -= payment;
	debt -= payment * (1.f - interestRate);
	totalDebtPaid += payment * (1.f - interestRate);
	interestPaid += payment * interestRate;

}