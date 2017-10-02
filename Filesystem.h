#pragma once
using namespace std;
namespace Filesystem {
	// Create a directory
	string CreateFolder(string path);
	// Get file by path
	bool GetOutput(string path, ofstream & ofs);
	bool GetInput(string path, ifstream & ifs);
	// Get list of file names in directory
	vector<string> FilesIn(string dir);
	string StripExtension(string filename);
	// Path operations
	string CombinePath(string A, string B);
}
