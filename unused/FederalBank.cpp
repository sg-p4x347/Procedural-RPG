#include "pch.h"
#include "FederalBank.h"
#include "Account.h"


FederalBank::FederalBank() : Bank() {
	m_interestRate = 0.05f;
	m_currency = 0;
}
void FederalBank::Update() {
	for (Account * account : m_accounts) {
		int debt = account->initialCredit - account->credit;
		if (debt > 0) {
			//------------DEBT CREATOIN----------------------------
			// take out a loan for the past year's credit
			//account->loans.push_back(Bank::Account::Loan(account->initialCredit - account->credit, m_interestRate));

			// take debt out of federal money supply
			m_currency -= debt;
			//-----------------------------------------------------
		}

		// calculate credit
		account->Update(m_interestRate);
	}
}
void FederalBank::Inflate(float stimulus) {
	for (Account * account : m_accounts) {
		account->credit = stimulus / m_accounts.size();
		account->initialCredit = account->credit;
	}
}
