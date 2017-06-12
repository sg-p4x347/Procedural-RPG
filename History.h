#pragma once

#include <map>
#include <list>
#include "City.h"
#include "Utility.h"
#include "NameGenerator.h"
#include "IdHandler.h"

using namespace DirectX;
using namespace std;
using namespace Utility;

namespace History
{
	enum Season {
		Spring = 0,
		Summer = 1,
		Fall = 2,
		Winter = 3
	};
	struct Date {
		Date();
		Date(int y, Season s);
		void operator++();
		void operator--();
		Date& operator+(const Date & other);
		Date& operator-(const Date & other);
		string ToString();
		int year;
		Season season;
	};
	struct Event {
		Event(string desc, Date s, Date e);
		string description;
		Date start;
		Date end;
	};	
};