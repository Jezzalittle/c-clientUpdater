#include "stdafx.h"
#include "FileManager.h"
#include "HashFile.h"
#include <experimental/filesystem> 
#include <assert.h>
#include <iostream>
#include <fstream>  
#include <thread>

namespace fs = std::experimental::filesystem::v1;

std::thread FileManager::readFromFileThread;
std::thread FileManager::deleteFileThread;


std::vector<HashFile> FileManager::CreateAllHashFiles(std::string& a_dir)
{
	std::vector<HashFile> hashFiles;
	int amoutOfFiles = 0;
	int currentAmount = 0;




	for (auto& fileDir : fs::recursive_directory_iterator(a_dir))
	{
		amoutOfFiles++;
	}

	std::string fileData;
	//iterators over ever folder and every subfolder
	for (auto& fileDir : fs::recursive_directory_iterator(a_dir))
	{
		std::ifstream inFile(fileDir);

		//std::string fileData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
		inFile >> fileData;
		inFile.close();

		std::string name = fileDir.path().string();

		if (name.substr(a_dir.size(), a_dir.size() - name.size()) != "\FileStructure.txt")
		{
			std::string nameDir = name.substr(a_dir.size(), a_dir.size() - name.size());
			hashFiles.push_back(HashFile(nameDir, fileData.c_str()));
		}

		currentAmount++;

	}

	return hashFiles;

}

void FileManager::CreateNewFileStucFileInDir(std::vector<HashFile>& hashFileArr, std::string& a_dir)
{
	std::ofstream outfile(a_dir + "\\FileStructure.txt");

	if (hashFileArr.size() == 0)
	{
		outfile << "";
		return;
	}
	for (auto& hashFile : hashFileArr)
	{
		outfile << hashFile.gethashValue() << std::endl;
	}

	outfile.close();

}


void FileManager::ReadFromfileStucFileByDir(std::string& a_dir)
{
	std::vector<HashFile> returnArr;
	std::ifstream inFile;
	std::string line;

	inFile.open(a_dir);
	if (inFile.is_open())
	{
		while (std::getline(inFile, line))
		{
			returnArr.push_back(HashFile(line));
		}
	}
}

bool FileManager::ReadFromfileStucFileByDir(const std::string& a_dir, CallbackFnc callbackFnc)
{
	if (readFromFileThread.joinable())
	{
		return false;
	}

	readFromFileThread = std::thread([=]()
	{

		std::vector<HashFile> returnArr;
		std::ifstream inFile;
		std::string line;

		inFile.open(a_dir);
		if (inFile.is_open())
		{
			while (std::getline(inFile, line))
			{
				returnArr.push_back(HashFile(line));
			}
		}

		callbackFnc(std::move(returnArr));
	});

	readFromFileThread.detach();

	return true;

}

std::vector<HashFile> FileManager::FindMissingFiles(std::vector<HashFile>& clientArr, std::vector<HashFile> serverArr)
{
	std::vector<HashFile> returnArr;
	bool found = false;

	for (HashFile& sfile : serverArr)
	{
		found = false;
		for (HashFile& cfile : clientArr)
		{
			if (sfile.gethashValue() == cfile.gethashValue())
			{
				found = true;
			}
		}
		if (found == false)
		{
			returnArr.push_back(sfile);
		}
	}

	return returnArr;
}

std::vector<HashFile> FileManager::FindFilesToDelete(std::vector<HashFile>& clientArr, std::vector<HashFile> serverArr)
{
	std::vector<HashFile> returnArr;
	bool found = false;

	for (HashFile& cfile : clientArr)
	{
		found = false;
		for (HashFile& sfile : serverArr)
		{
			if (sfile.gethashValue() == cfile.gethashValue())
			{
				found = true;
			}
		}
		if (found == false)
		{
			returnArr.push_back(cfile);
		}
	}
	return returnArr;
}

bool FileManager::DeleteFilesFromDir(std::string orginalDir)
{

	if (std::remove(orginalDir.c_str()) != 0)
	{
		bool failed = !std::ifstream(orginalDir);
		if (failed == true)
		{
			std::cout << "cant find file location" << std::endl;
		}
		std::cout << "error deleting file" << orginalDir << std::endl;
	}
	else
	{
		std::cout << "deleting file from " << orginalDir << std::endl;
	}

	return true;
}


