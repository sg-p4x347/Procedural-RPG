#include "pch.h"
#include "XmlParser.h"


XmlParser::XmlParser()
{
	
}
enum ParsingMode {
	Undefined,
	Opening,
	Children,
	Closing,
	AttributeName,
	AttributeValue
};
XmlParser::XmlParser(istream & ifs)
{
	char last = '\0';

	string attributeName = "";
	string attributeValue = "";
	int quoteCount = 0;
	ParsingMode mode = Undefined;
	while (!ifs.eof()) {
		char ch = ifs.get();
		if (mode == Undefined) {
			if (ch == '<') {
				// parse opening tag name
				mode = Opening;
			}
		}
		else if (mode == Opening) {
			if (!iswspace(ch) && ch != '>') {
				m_tag += ch;
			} else if (ch == '>') {
				// Parse children
				mode = Children;
			}
			else if (iswspace(ch)) {
				mode = AttributeName;
			}
		} 
		else if (mode == AttributeName) {
			
			if (ch == '=') {
				mode = AttributeValue;
			}
			else if (ch == '>') {
				mode = Children;
			} else if (!iswspace(ch)) {
				attributeName += ch;
			}
		}
		else if (mode == AttributeValue) {
			if (ch == '"') {
				if (++quoteCount % 2 == 0) {
					m_attributes.insert(std::make_pair(attributeName, attributeValue));
					attributeName = "";
					attributeValue = "";
					mode = AttributeName;
				}
			} else if (attributeValue != "" || !iswspace(ch)) {
				attributeValue += ch;
			}
		}
		else if (mode == Children) {
			if (ch == '<') {
				if ((char)ifs.peek() == '/') {
					mode = Closing;
				}
				else {
					// Recursively parse children
					ifs.putback(ch);
					m_children.push_back(std::make_shared<XmlParser>(ifs));
				}
			}
		}
		else if (mode == Closing) {
			if (ch == '>') {
				break;
			}
		}
	}
}

string & XmlParser::GetTag()
{
	return m_tag;
}

map<string, string>& XmlParser::GetAttributes()
{
	return m_attributes;
}

vector<shared_ptr<XmlParser>>& XmlParser::GetChildren()
{
	return m_children;
}

