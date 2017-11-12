#pragma once
struct Loan {
	Loan(int ammount, int interest);
	// update blance due
	void Update();
	void Payment(int & ammount);
	// Properties
	int principal;
	int balance;
	int debtPaid;
	float interestRate;
};

