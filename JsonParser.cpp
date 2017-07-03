#include "pch.h"
#include "JsonParser.h"
#include <iomanip>

JsonParser::JsonParser() {
	m_type = JsonType::object;
}
JsonParser::JsonParser(const JsonParser & other)
{
	m_array = other.m_array;
	m_object = other.m_object;
	m_type = other.m_type;
	m_value = other.m_value;
}
JsonParser::JsonParser(JsonType type)
{
	m_type = type;
}

JsonParser::JsonParser(const string & json)
{
	Parse(json);
}

JsonParser::JsonParser(istream & fileStream) :
	JsonParser(string(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>()))
{
	
}
string JsonParser::GetJsonScope(stringstream & stream) const
{
	stringstream scope = stringstream("");
	bool inString = false;
	bool inComment = false;
	bool inPrimitive = false;
	int scopeCount = 0;

	char ch;
	while (stream.get(ch))
	{
		if (!inString) {
			if (ch == '/') {
				inComment = true;
			} else if (ch == '\n') {
				inComment = false;
			}
		}
		if (!inComment) {
			if (inString) scope << ch;

			switch (ch)
			{
			// opening scope
			case '[':
			case '{':
				if (!inString) {
					scopeCount++;
				}
				scope << ch;
				break;
			case '"': // opens and closes scope
				if (!inString) {
					scopeCount++;
					scope << ch;
				} else {
					scopeCount--;
				}
				inString = !inString;
			
				break;
			// closing scope
			case ']':
			case '}':
				if (!inString) {
					scopeCount--;
				}
				if (inPrimitive) {
					inPrimitive = false;
					scopeCount--;
				}
				if (scopeCount < 0) {
					stream.putback(ch);
				}
				scope << ch;
			
				break;
			// ignore
			case ':':
			case ',':
				
				if (inPrimitive) {
					inPrimitive = false;
					scopeCount--;
					stream.putback(ch);
				} else {
					scope << ch;
				}
				break;
		
			// anything else
			default:
				if (inPrimitive) {
					scope << ch;
				} else if (!inString && !isspace(ch)) {
					scopeCount++;
					inPrimitive = true;
					scope << ch;
				}
				break;
			}
			if ((scope.str().length() > 0 && scopeCount <= 0) || stream.eof() || !stream.good()) {
				stream.clear();
				return (scopeCount == 0 ? scope.str() : "");
			}
		}
	}
}
void JsonParser::GetJsonScope(istream & in, ostream & out) const {
	char ch;
	in >> ch;
	if (!in.eof() && !isspace(ch)) {
		switch (ch) {
		case '{': out << ch; GetObjectScope(in, out); break;
		case '[': out << ch; GetArrayScope(in, out); break;
		case '"': out << ch; GetStringScope(in, out); break;
		case '/': GetCommentScope(in, out); GetJsonScope(in, out); break;
		case ':': 
		case ',': out << ch; break;
		default: out << ch; GetPrimitiveScope(in, out); break;
		}
	}
}
void JsonParser::GetObjectScope(istream & in, ostream & out) const {
	char ch;
	while (in.get(ch)) {
		if (ch == '}') {
			out << ch;
			return;
		} else {
			in.putback(ch);
			GetJsonScope(in, out);
		}
	}
}
void JsonParser::GetArrayScope(istream & in, ostream & out) const {
	char ch;
	while (in.get(ch)) {
		if (ch == ']') {
			out << ch;
			return;
		}
		else {
			in.putback(ch);
			GetJsonScope(in, out);
		}
	}
}
void JsonParser::GetStringScope(istream & in, ostream & out) const {
	char ch;
	while (in.get(ch)) {
		out << ch;
		if (ch == '"') return;
	}
}
void JsonParser::GetPrimitiveScope(istream & in, ostream & out) const {
	char ch;
	while (in.get(ch)) {
		if (!isspace(ch)) {
			switch (ch) {
			case '}':
			case ']':
			case ',': 
			case '/': in.putback(ch); return;  break;
			default: out << ch; break;
			}
		}
	}
}
void JsonParser::GetCommentScope(istream & in, ostream & out) const {
	char ch;
	while (in.get(ch)) {
		if (ch == '\n') return;
	}
}
string JsonParser::DeQuote(string str) const
{
	if (str.length() > 0) {
		if (str.front() == '\"')
		{
			str = str.substr(1, str.length());
		}
		if (str.back() == '\"')
		{
			str = str.substr(0, str.length() - 1);
		}
	}
	return str;
}

string JsonParser::Quotate(string str) const
{
	return '"' + str + '"';
}

void JsonParser::Parse(string json)
{
	if (json.length() > 0) {
		stringstream stream = stringstream(json);
		ostringstream out;
		GetJsonScope(stream, out);
		json = out.str(); // trim the edges
		switch (json[0]) {
		case '[': ParseArray(json); break;
		case '{': ParseObject(json); break;
		default: ParsePrimitive(json); break;
		}
	}
}
void JsonParser::ParsePrimitive(string json) {
	m_type = JsonType::primitive;
	m_value = json;
}
//===================================
// JSON Object
//===================================

void JsonParser::ParseObject(string json)
{
	m_type = JsonType::object;
	stringstream stream = stringstream(json);
	char token;
	string key;
	while (stream >> token && !stream.eof()) {
		if (token == '"') {
			stream.putback(token);
			ostringstream out;
			GetJsonScope(stream, out);
			key = DeQuote(out.str());
		}
		else if (token == ':')
		{
			if (m_object.find(key) == m_object.end())
			{
				//string value = DeQuote(GetJsonScope(stream));
				ostringstream out;
				GetJsonScope(stream, out);
				JsonParser value(out.str());
				m_object.insert(std::pair<string, JsonParser>(key, value)); // adding the key with the value (obtained from getting the next json scope)
			}
		}
	}
}

JsonParser JsonParser::operator[](const string & key) const
{
	return JsonParser(m_object.at(key));
}

void JsonParser::Export(ostream & fileStream) const
{
	switch (m_type) {
		case JsonType::object : ExportObject(fileStream); break;
		case JsonType::array: ExportArray(fileStream); break;
		case JsonType::primitive: ExportPrimitive(fileStream); break;
	}
}

string JsonParser::ToString() const
{
	stringstream ss;
	Export(ss);
	return ss.str();
}

vector<string> JsonParser::GetKeys() const
{
	vector<string> keys = vector<string>();
	for (std::map<string, JsonParser>::const_iterator iter = m_object.begin(); iter != m_object.end(); ++iter) {
		keys.push_back(iter->first);
	}
	return keys;
}
void JsonParser::ParseArray(string json)
{
	m_type = JsonType::array;
	stringstream stream = stringstream(json);
	char token;
	if (stream >> token && token == '[') {
		// get the first element
		token = ',';
		do {
			if (token == ',') {
				//m_array.push_back(DeQuote(GetJsonScope(stream)));
				ostringstream out;
				GetJsonScope(stream, out);
				m_array.push_back(out.str());
			}
			else if (token == ']') {
				break;
			}
			// get any more elements
		} while (stream >> token && !stream.eof());
	}
}
void JsonParser::ExportObject(ostream & fileStream) const
{
	fileStream << '{';
	bool first = true;
	for (std::map<string, JsonParser>::const_iterator iter = m_object.begin(); iter != m_object.end(); ++iter) {
		if (!first) fileStream << ',';
		fileStream << Quotate(iter->first);
		fileStream << ':';
		iter->second.Export(fileStream);
		first = false;
	}
	fileStream << '}';
}

//===================================
// JSON Array
//===================================
JsonParser JsonParser::operator[](const int & index) const
{
	return JsonParser(m_array.at(index));
}

vector<JsonParser> JsonParser::GetElements() const
{
	return m_array;
}

int JsonParser::Count() const
{
	return m_array.size();
}
void JsonParser::ExportArray(ostream & fileStream) const
{
	fileStream << '[';
	bool first = true;
	for (JsonParser element : m_array) {
		if (!first) fileStream << ',';
		element.Export(fileStream);
		first = false;
	}
	fileStream << ']';
}

void JsonParser::ExportPrimitive(ostream & fileStream) const
{
	fileStream << m_value;
}
