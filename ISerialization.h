#pragma once
class ISerialization
{
public:
	virtual void Import(std::istream & ifs) = 0;
	virtual void Export(std::ostream & ofs) = 0;

	template <typename T>
	inline void Serialize(T & value, std::ostream & ofs) {
		char* bytes = new char[sizeof (T)];
		memcpy(bytes, &value, sizeof(T));
		ofs.write(bytes, sizeof(T));
		/*for (int i = 0; i < sizeof(T); i++) {
			ofs << bytes[i];
		}*/
		delete[] bytes;
	}
	template <>
	inline void Serialize(string & value, std::ostream & ofs) {
		ofs.write(value.c_str(), sizeof(char) * (value.length() + 1));
	}
	template <typename T>
	inline void DeSerialize(T & value, std::istream & ifs) {
		char* bytes = new char[sizeof(T)];
		ifs.read(bytes, sizeof(T));
		memcpy(&value, bytes, sizeof(T));
		
		delete[] bytes;
	}
	template <>
	inline void DeSerialize(string & value, std::istream & ifs) {
		std::getline(ifs, value, '\0');
	}
};

