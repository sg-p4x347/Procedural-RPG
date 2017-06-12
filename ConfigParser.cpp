#include "pch.h"
#include "ConfigParser.h"

ConfigParser::ConfigParser(string path) {
	m_path = path;
	m_file = ifstream(path);

	// STORE ALL VALUES IN MEMORY

	//string key = "";
	//string value = "";
	//char ch;
	//// get the property name
	//while (m_file.get(ch) && !m_file.eof()) {
	//	if (ch == '=') {
	//		// add the key to the key vector
	//		m_keys.push_back(key);
	//		key = "";
	//		// get the value for the key
	//		while (m_file.get(ch) && !m_file.eof()) {
	//			if (ch == ';') {
	//				m_values.push_back(value);
	//				value = "";
	//				break;
	//			}
	//			else if (ch != '\t' && ch != '\n') {
	//				value += ch;
	//			}
	//		}
	//	}
	//	else if (ch == '/') {
	//		// ingore comment
	//		string comment;
	//		getline(m_file, comment);
	//	}
	//	else if (isalpha(ch) || ch == '_') {
	//		key += ch;
	//	}
	//}
}


ConfigParser::~ConfigParser() {
}

string ConfigParser::GetString(string prop) {
	m_file.seekg(0, ios::beg);
	size_t pos = 0;
	string token;
	string name = "";
	string value = "";
	prop += '.'; // fix the delimeter for the last property name
	while ((pos = prop.find('.')) != string::npos) {
		token = prop.substr(0, pos);

		char ch;
		// get the property name
		while (m_file.get(ch) && !m_file.eof()) {
			if (ch == '=') {
				if (name == token) {
					// get the value for the property
					while (true) {
						m_file.get(ch);
						if (ch == ';' || ch == '{') {
							goto nextProp;
						}
						else if (ch != ' ' && ch != '\t' && ch != '\n') {
							value += ch;
						}
					}
					break;
				}
				else {
					name = "";
				}

			}
			else if (ch == '/') {
				// ingore comment
				string comment;
				getline(m_file, comment);
			} else if (ch == '{') {
				// run until next member name
				while (m_file.get(ch) && !m_file.eof() && ch != '}') ;
			}
			else if (ch == '}') {
				// didn't find the property
				return "";
			}
			else if (isalpha(ch) || ch == '_') {
				name += ch;
			}
		}
	nextProp:
		name = "";
		prop.erase(0, pos + 1);
	}
	return value;
}
vector<string> ConfigParser::GetStringVector(string prop)
{
	vector<string> vec;
	string list = GetString(prop);


	return vec;
}
double ConfigParser::GetDouble(string prop) {
	return atof(GetString(prop).c_str());
}

int ConfigParser::GetInt(string prop) {
	return atoi(GetString(prop).c_str());
}

vector<string> ConfigParser::GetPropertyNames(string prop)
{
	vector<string> names = vector<string>();
	bool insideObject = false;

	m_file.seekg(0, ios::beg);
	size_t pos = 0;
	string token;
	string name = "";
	prop += '.'; // fix the delimeter for the last property name
	while ((pos = prop.find('.')) != string::npos) {
		token = prop.substr(0, pos);

		char ch;
		// get the property name
		beginSearch:
		while (m_file.get(ch) && !m_file.eof()) {
			if (ch == '=') {
				if (insideObject) {
					names.push_back(name);
				}
				if (name == token) {
					// get the value for the property
					while (true) {
						m_file.get(ch);
						if (ch == ';' || ch == '{') {
							insideObject = true;
							name = "";
							goto beginSearch;
						}
					}
					break;
				}
				else {
					name = "";
				}

			}
			else if (ch == '/') {
				// ingore comment
				string comment;
				getline(m_file, comment);
			}
			else if (ch == '{') {
				// run until next member name
				while (m_file.get(ch) && !m_file.eof() && ch != '}');
			}
			else if (ch == '}') {
				// didn't find the property
				return names;
			}
			else if (isalpha(ch) || ch == '_') {
				name += ch;
			}
		}
	nextProp:
		name = "";
		prop.erase(0, pos + 1);
	}
	return names;
}
