#pragma once
class XmlParser
{
public:
	XmlParser();
	XmlParser(istream & filestream);
private:
	vector<shared_ptr<XmlParser>> m_children;
	string m_tag;
	map<string, string> m_attributes;


};

