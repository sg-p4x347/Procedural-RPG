#include "pch.h"
#include "Loan.h"

Loan::Loan(int ammount, int interest) {
	principal = ammount;
	balance = -principal;
	debtPaid = 0;
	interestRate = interest;
}
// update blance due
void Loan::Update() {
	balance -= balance * interestRate;
}
void Loan::Payment(int & ammount) {
	int payment = std::min(balance, ammount);
	ammount -= payment;
	balance += payment;
	debtPaid = std::min(principal, debtPaid + payment); // maxes out on the principal
}
