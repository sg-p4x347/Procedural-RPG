#include "pch.h"
#include "Bank.h"


Bank::Bank() {
	m_accounts = std::vector<Account *>();
	m_currency = 0;
}
Account * Bank::AddAccount() {
	Account * account = new Account();
	m_accounts.push_back(account);
	return account;
}
