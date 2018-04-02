#pragma once
#include <iostream>
#include <vector>

class HashFile;

static class FileManager 
{
public:

	
	static std::vector<HashFile> CreateAllHashFiles(std::string a_dir);
	static void CreateNewFileStucFileInDir(std::vector<HashFile> hashFileArr, std::string a_dir);
	static std::vector<HashFile> ReadFromfileStucFileByDir(std::string a_dir);
	static std::vector<HashFile> FindMissingFiles(std::vector<HashFile> clientArr, std::vector<HashFile> serverArr);

};

