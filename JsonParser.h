#pragma once
#include <sstream>
#include <map>
#include <vector>

using namespace std;
enum JsonType {
	object,
	array,
	primitive,
	null
};
class JsonParser
{
public:
	
	JsonParser();
	JsonParser(const JsonParser & other);
	JsonParser(JsonType type);
	JsonParser(const string & json);
	JsonParser(istream & filestream);
	void Export(std::ostream & ofs) const;
	string ToString() const;
	// explicit conversions
	explicit operator short() const { return To<short>(); }
	explicit operator unsigned short() const { return To<unsigned short>(); }
	explicit operator int() const { return To<int>(); }
	explicit operator unsigned int() const { return To<unsigned int>(); }
	explicit operator long() const { return To<long>(); }
	explicit operator unsigned long() const { return To<unsigned long>(); }
	explicit operator float() const { return To<float>(); }
	explicit operator double() const { return To<double>(); }
	explicit operator bool() const { return To<bool>(); }
	explicit operator string() const { return To<string>(); }
	//===================================
	// JSON Primitive
	//===================================
	template<typename T>
	inline T To() const
	{
		stringstream ss(DeQuote(m_value));
		T value = T();
		ss >> std::boolalpha >> value;
		return value;
	}
	template<>
	inline string To() const
	{
		return DeQuote(m_value);
	}
	template<typename T>
	inline vector<T> ToVector()
	{
		vector<T> vec;
		for (JsonParser & jp : m_array) {
			T JSONable;
			JSONable.Import(jp);
			vec.push_back(JSONable);
		}
		return vec;
	}

	template<typename T>
	inline void Store(T & var) const
	{
		stringstream ss(DeQuote(m_value));
		ss >> std::boolalpha >> var;
	}
	//===================================
	// JSON Object
	//===================================
	
	JsonParser operator[](const string & key) const;
	//// get a value from an object
	//vector<string> GetKeys() const;
	//template<typename T>
	//inline T Get(const string & key) const
	//{
	//	stringstream ss(DeQuote(m_object.at(key)));
	//	T value = T();
	//	ss >> value;
	//	return value;
	//}
	vector<string> GetKeys() const;
	// add/set a value from a variable
	template<typename T>
	inline void Set(const string & key, const T & value)
	{
		stringstream ss;
		ss << value;
		m_object[key] = ss.str();
	}
	template<>
	inline void Set<string>(const string & key, const string & value) {
		m_object[key] = Quotate(value);
	}
	template<>
	inline void Set<JsonParser>(const string & key, const JsonParser & value) {
		m_object[key] = value.ToString();
	}
	//===================================
	// JSON Array
	//===================================
	
	JsonParser operator[] (const int & index) const;

	// get a value from an array
	vector<JsonParser> GetElements() const;
	int Count() const;
	// Gets the element at the specified index as a specified type
	/*template<typename T>
	inline T Get(const int & index) const
	{
		stringstream ss(DeQuote(m_array[index]));
		T value = T();
		ss >> value;
		return value;
	}*/
	// Gets the index of the specified element
	template<typename T>
	inline int IndexOf(const T & value) const
	{
		for (int i = 0; i < m_array.size(); i++) {
			if (m_array[i].To<T>() == value) return i;
		}
		return -1; // not found
	}

	// add/set a value from a variable
	template<typename T>
	inline void Set(const int & index, const T & value)
	{
		stringstream ss;
		ss << value;
		m_array[index] = ss.str();
	}
	template<>
	inline void Set<string>(const int & index, const string & value) {
		m_array[index] = Quotate(value);
	}
	template<>
	inline void Set<JsonParser>(const int & index, const JsonParser & value) {
		m_array[index] = value.ToString();
	}
	template<typename T>
	inline void Add(const T & value)
	{
		m_array.push_back(JsonParser());
		Set<T>(m_array.size() - 1, value);
	}
private:
	string GetJsonScope(stringstream & ss) const;
	void GetJsonScope(istream & in, ostream & out) const;
	void GetObjectScope(istream & in, ostream & out) const;
	void GetArrayScope(istream & in, ostream & out) const;
	void GetStringScope(istream & in, ostream & out) const;
	void GetPrimitiveScope(istream & in, ostream & out) const;
	void GetCommentScope(istream & in, ostream & out) const;

	string DeQuote(string str) const;
	string Quotate(string str) const;
	void Parse(string json);
	void ParseObject(string json);
	void ParseArray(string json);
	void ParsePrimitive(string json);


	void ExportObject(ostream & fileStream) const;
	void ExportArray(ostream & fileStream) const;
	void ExportPrimitive(ostream & fileStream) const;

	JsonType m_type;
	map<string, JsonParser> m_object;
	vector<JsonParser> m_array;
	string m_value;
};

