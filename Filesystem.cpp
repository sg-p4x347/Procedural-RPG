#include "pch.h"
#include "Filesystem.h"
#include <experimental/filesystem>
#include <filesystem>

namespace Filesystem = std::experimental::filesystem::v1;

namespace Filesystem {
	string CreateFolder(string path)
	{
		CreateDirectory(wstring(path.begin(), path.end()).c_str(), NULL);
		return path;
	}
	bool GetOutput(string path, ofstream & ofs)
	{
		ofs = ofstream(path);
		return ofs.is_open();
	}
	bool GetInput(string path, ifstream & ifs)
	{
		ifs = ifstream(path);
		return ifs.is_open();
	}
	vector<string> FilesIn(string dir)
	{
		vector<string> paths;
		for (auto & p : directory_iterator(dir)) {
			paths.push_back(p.path().filename().string());
		}
	}
	string StripExtension(string path) {
		string filename = "";
		for (int i = path.size() - 1; i >= 0; i--) {
			if (path[i] == '.') {
				return path.substr(0,i);
			}
		}
	}
	string CombinePath(string A, string B)
	{
		return A + '/' + B;
	}
}
