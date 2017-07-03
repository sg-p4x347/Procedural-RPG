#pragma once
class ISerialization
{
public:
	virtual void Import(std::ifstream & ifs) = 0;
	virtual void Export(std::ofstream & ofs) = 0;

	template <typename T>
	inline void Serialize(T & value, std::ofstream & ofs) {
		byte* bytes = new byte[sizeof (T)];
		memcpy(bytes, &value, sizeof(T));
		for (int i = 0; i < sizeof(T); i++) {
			ofs << bytes[i];
		}
		delete[] bytes;
	}
	template <typename T>
	inline void DeSerialize(T & value, std::ifstream & ifs) {
		byte* bytes = new byte[sizeof(T)];
		for (int i = 0; i < sizeof(T); i++) {
			ifs >> bytes[i];
		}
		memcpy(&value, bytes, sizeof(T));
		
		delete[] bytes;
	}
};

