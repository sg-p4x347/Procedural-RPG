#pragma once
class ISerialization
{
public:
	virtual void Import(std::istream & ifs) = 0;
	virtual void Export(std::ostream & ofs) = 0;

	template <typename T>
	inline void Serialize(T & value, std::ostream & ofs) {
		ofs.write((const char *)&value, sizeof(T));
	}
	template <>
	inline void Serialize(string & value, std::ostream & ofs) {
		ofs.write(value.c_str(), sizeof(char) * (value.length() + 1));
	}
	template <typename T>
	inline void DeSerialize(T & value, std::istream & ifs) {
		ifs.read((char *)&value, sizeof(T));
	}
	template <>
	inline void DeSerialize(string & value, std::istream & ifs) {
		std::getline(ifs, value, '\0');
	}
};

