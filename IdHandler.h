#pragma once
#include <vector>
#include <exception>
using namespace std;

class IDhandler
{
public:
	IDhandler();
	~IDhandler();
	int GetID();
	template <typename T> T & GetObject(int ID, const vector<T> list);
private:
	int nextID;
};


