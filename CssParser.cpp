#include "pch.h"
#include "CssParser.h"
#include <sstream>

CssParser::CssParser(stringstream & ifs)
{
	std::pair<string, string> style;
	while (ParseStyle(ifs, style)) {
		m_styles.insert(style);
	}
}

bool CssParser::ParseStyle(stringstream & ifs, pair<string, string> & pair)
{
	string key = "";
	while (!ifs.eof()) {
		char ch = ifs.get();
		if (ch != ':') {
			key += ch;
		}
		else {
			pair.first = key;
			// begin parsing the value
			string value = "";
			while (!ifs.eof()) {
				ch = ifs.get();
				if (ch == ';') {
					pair.second = value;
					return true;
				}
				else {
					value += ch;
				}
			}
		}
	}
	return false;
}
