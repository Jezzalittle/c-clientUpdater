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


	//Take in a Directory and for each file and file underneath get the path and content of that file and hash it, once all the files are hashed return a vector of hashFiles 
	static std::vector<HashFile> CreateAllHashFiles(std::string& a_dir);


	//Take in a vector of hashFiles and make a FileStructre file with all of the hash values
	static void CreateNewFileStucFileInDir(std::vector<HashFile>& hashFileArr, std::string& a_dir);


	//Make a hashFile from one single FileStruture file
	static void ReadFromfileStucFileByDir(std::string& a_dir);

	//Make a hashFile from one single FileStruture file returns a bool (Threaded), callback function called when done
	static bool ReadFromfileStucFileByDir(const std::string& a_dir, CallbackFnc callbackFnc);

	//Take in two hashFile vectors and find which files that are in the 2 argument that are not in the 1 argument	
	static std::vector<HashFile> FindMissingFiles(std::vector<HashFile>& clientArr, std::vector<HashFile> serverArr);

	//Take in two hashFile vectors and find which files that are in the 1 argument that are not in the 2 argument	
	static std::vector<HashFile> FindFilesToDelete(std::vector<HashFile>& clientArr, std::vector<HashFile> serverArr);

	//Take in a Directory and Delete the file in that Directory 
	static bool DeleteFilesFromDir (std::string orginalDir);


private:

	static std::thread readFromFileThread;
	static std::thread deleteFileThread;

};

