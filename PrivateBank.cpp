#include "pch.h"
#include "PrivateBank.h"


PrivateBank::PrivateBank(Account * account) : m_account(account), Bank() {
}
void PrivateBank::Update() {
	m_interestRate = m_account->interestRate;
	for (Account * account : m_accounts) {
		// add interest paid to the bank's income & currency
		m_account->income += account->interestPaid;
		m_currency += account->interestPaid + account->debtPaid;



		float debt = account->initialCredit - account->credit;
		if (debt > 0) {
			//------------DEBT CREATOIN----------------------------
			// take out a loan for the past year's credit
			//account->loans.push_back(Bank::Account::Loan(account->initialCredit - account->credit, m_interestRate));

			// take loan from currency and credit supply of the bank
			m_account->credit -= std::max(debt - m_currency, 0.f);
			m_currency -= std::min(debt, m_currency);
			//-----------------------------------------------------
		}

		// update the account
		account->Update(m_interestRate);
	}
	// compensate the bank's account
	m_account->Compensate(m_currency);
}
