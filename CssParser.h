#pragma once
class CssParser
{
public:
	CssParser(string css);
	map<string, string> & GetStyles();
private:
	bool ParseStyle(stringstream & ifs, pair<string,string> & pair);
	map<string, string> m_styles;
};

