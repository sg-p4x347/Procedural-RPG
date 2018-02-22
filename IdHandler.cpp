#include "pch.h"
#include "IdHandler.h"


IDhandler::IDhandler()
{
	nextID = 0;
}


IDhandler::~IDhandler()
{
}

int IDhandler::GetID()
{
	return nextID++;
}
template<typename T>
T & IDhandler::GetObject(int ID, const vector<T> list)
{
	try {
		for (T object : list) {
			if (object.ID == ID) {
				return object;
			}
		}
	}
	catch (std::exception& e) {
		cerr << "Invalid use of IdHandler: " << e.what() << endl;
	}
	return T();
}
