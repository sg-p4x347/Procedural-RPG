#include "pch.h"
#include "History.h"

namespace History {
	Date::Date()
	{
		year = 0;
		season = Spring;
	}
	Date::Date(int y, Season s)
	{
		year = y;
		season = s;
	}
	Event::Event(string desc, Date s, Date e)
	{
		description = desc;
		start = s;
		end = e;
	}
	void Date::operator++()
	{
		year++;
	}
	void Date::operator--()
	{
		year--;
	}
	Date & Date::operator+(const Date & other)
	{
		return Date(year + other.year,season);
	}
	Date & Date::operator-(const Date & other)
	{
		return Date(year - other.year, season);
	}
	string Date::ToString()
	{
		string seasonName;
		switch (season) {
		case Spring: seasonName = "Spring"; break;
		case Summer: seasonName = "Summer"; break;
		case Fall: seasonName = "Fall"; break;
		case Winter: seasonName = "Winter"; break;
		}
		return seasonName + " of " + std::to_string(year);
	}
}
