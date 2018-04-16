#pragma once
class XmlParser
{
public:
	XmlParser();
	XmlParser(istream & filestream);

	string & GetTag();
	map<string, string> & GetAttributes();
	vector<shared_ptr<XmlParser>> & GetChildren();
	

private:
	string m_tag;
	map<string, string> m_attributes;
	vector<shared_ptr<XmlParser>> m_children;
	
	


};

