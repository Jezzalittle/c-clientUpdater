#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include <thread>

class HashFile;

static class FileManager 
{
public:
	using HashList = std::vector<HashFile>;
	using CallbackFnc = std::function<void(HashList&& hashArray)>;

	
	static std::vector<HashFile> CreateAllHashFiles(std::string a_dir);
	static void CreateNewFileStucFileInDir(std::vector<HashFile> hashFileArr, std::string a_dir);
	void ReadFromfileStucFileByDir(std::string a_dir);
	static bool ReadFromfileStucFileByDir(std::string a_dir, CallbackFnc callbackFnc);
	static std::vector<HashFile> FindMissingFiles(std::vector<HashFile> clientArr, std::vector<HashFile> serverArr);
	static std::vector<HashFile> FindFilesToDelete(std::vector<HashFile> clientArr, std::vector<HashFile> serverArr);


private:

	static std::thread m_readFromFileThread;

};

