#pragma once
class CssParser
{
public:
	CssParser(stringstream & ifs);
	
private:
	bool ParseStyle(stringstream & ifs, pair<string,string> & pair);
	map<string, string> m_styles;
};

