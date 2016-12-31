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
	string name = "";
	string value = "";
	char ch;
	// get the property name
	while (m_file.get(ch) && !m_file.eof()) {
		if (ch == '=') {
			if (name == prop) {
				// get the value for the property
				while (true) {
					m_file.get(ch);
					if (ch == ';') {
						return value;
					} else if (ch != '\t' && ch != '\n') {
						value += ch;
					}
				}
				break;
			} else {
				name = "";
			}

		} else if (ch == '/') {
			// ingore comment
			string comment;
			getline(m_file, comment);
		} else if (isalpha(ch) || ch == '_') {
			name += ch;
		}
	}
	return "";
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
