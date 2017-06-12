#pragma once

using namespace std;

class ConfigParser
{
public:
	ConfigParser(string path);
	~ConfigParser();
	// get the value of a property
	string GetString(string prop);
	vector<string> GetStringVector(string prop);

	double GetDouble(string prop);
	vector<double> GetDoubleVector(string prop);

	int GetInt(string prop);
	vector<int> GetIntVector(string prop);
	// get all property names in an object
	vector<string> GetPropertyNames(string prop);
private:
	string m_path;
	ifstream m_file;
	// store key-value pairs
	//vector<string> m_keys;
	//vector<string> m_values;
};

