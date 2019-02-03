#include "pch.h"
#include "Filesystem.h"
#include <experimental/filesystem>
#include <filesystem>

using namespace std::experimental::filesystem::v1;

	string FileSystemHelpers::CreateFolder(string path)
	{
		CreateDirectory(wstring(path.begin(), path.end()).c_str(), NULL);
		return path;
	}
	bool FileSystemHelpers::GetOutput(string path, ofstream & ofs)
	{
		ofs = ofstream(path);
		return ofs.is_open();
	}
	bool FileSystemHelpers::GetInput(string path, ifstream & ifs)
	{
		ifs = ifstream(path);
		return ifs.is_open();
	}
	vector<string> FileSystemHelpers::FilesIn(string dir)
	{
		vector<string> paths;
		for (auto & p : directory_iterator(dir)) {
			paths.push_back(p.path().filename().string());
		}
		return paths;
	}
	/*string FileSystemHelpers::StripExtension(string path) {
		for (int i = path.size() - 1; i >= 0; i--) {
			if (path[i] == '.') {
				return path.substr(0,i);
			}
		}
		return "";
	}*/
	string FileSystemHelpers::CombinePath(string A, string B)
	{
		return A + '/' + B;
	}

	string FileSystemHelpers::FilenameWithoutExtension(Filesystem::path path)
	{
		string stringPath = path.filename().string();
		return stringPath.substr(0,stringPath.find('.'));
	}
